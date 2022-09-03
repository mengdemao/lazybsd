/*-
 * Copyright (c) 2014 Andrew Turner
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/ktr.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/proc.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/sysent.h>
#ifdef KDB
#include <sys/kdb.h>
#endif

#include <vm/vm.h>
#include <vm/pmap.h>
#include <vm/vm_kern.h>
#include <vm/vm_map.h>
#include <vm/vm_param.h>
#include <vm/vm_extern.h>

#include <machine/frame.h>
#include <machine/md_var.h>
#include <machine/pcb.h>
#include <machine/pcpu.h>
#include <machine/undefined.h>

#ifdef KDTRACE_HOOKS
#include <sys/dtrace_bsd.h>
#endif

#ifdef VFP
#include <machine/vfp.h>
#endif

#ifdef KDB
#include <machine/db_machdep.h>
#endif

#ifdef DDB
#include <ddb/db_output.h>
#endif

extern register_t fsu_intr_fault;

/* Called from exception.S */
void do_el1h_sync(struct thread *, struct trapframe *);
void do_el0_sync(struct thread *, struct trapframe *);
void do_el0_error(struct trapframe *);
void do_serror(struct trapframe *);
void unhandled_exception(struct trapframe *);

static void print_registers(struct trapframe *frame);

int (*dtrace_invop_jump_addr)(struct trapframe *);

typedef void (abort_handler)(struct thread *, struct trapframe *, uint64_t,
    uint64_t, int);

static abort_handler align_abort;
static abort_handler data_abort;
static abort_handler external_abort;

static abort_handler *abort_handlers[] = {
	[ISS_DATA_DFSC_TF_L0] = data_abort,
	[ISS_DATA_DFSC_TF_L1] = data_abort,
	[ISS_DATA_DFSC_TF_L2] = data_abort,
	[ISS_DATA_DFSC_TF_L3] = data_abort,
	[ISS_DATA_DFSC_AFF_L1] = data_abort,
	[ISS_DATA_DFSC_AFF_L2] = data_abort,
	[ISS_DATA_DFSC_AFF_L3] = data_abort,
	[ISS_DATA_DFSC_PF_L1] = data_abort,
	[ISS_DATA_DFSC_PF_L2] = data_abort,
	[ISS_DATA_DFSC_PF_L3] = data_abort,
	[ISS_DATA_DFSC_ALIGN] = align_abort,
	[ISS_DATA_DFSC_EXT] =  external_abort,
};

static __inline void
call_trapsignal(struct thread *td, int sig, int code, void *addr, int trapno)
{
	ksiginfo_t ksi;

	ksiginfo_init_trap(&ksi);
	ksi.ksi_signo = sig;
	ksi.ksi_code = code;
	ksi.ksi_addr = addr;
	ksi.ksi_trapno = trapno;
	trapsignal(td, &ksi);
}

int
cpu_fetch_syscall_args(struct thread *td)
{
	struct proc *p;
	register_t *ap, *dst_ap;
	struct syscall_args *sa;

	p = td->td_proc;
	sa = &td->td_sa;
	ap = td->td_frame->tf_x;
	dst_ap = &sa->args[0];

	sa->code = td->td_frame->tf_x[8];

	if (__predict_false(sa->code == SYS_syscall || sa->code == SYS___syscall)) {
		sa->code = *ap++;
	} else {
		*dst_ap++ = *ap++;
	}

	if (__predict_false(sa->code >= p->p_sysent->sv_size))
		sa->callp = &p->p_sysent->sv_table[0];
	else
		sa->callp = &p->p_sysent->sv_table[sa->code];

	KASSERT(sa->callp->sy_narg <= nitems(sa->args),
	    ("Syscall %d takes too many arguments", sa->code));

	memcpy(dst_ap, ap, (MAXARGS - 1) * sizeof(register_t));

	td->td_retval[0] = 0;
	td->td_retval[1] = 0;

	return (0);
}

#include "../../kern/subr_syscall.c"

/*
 * Test for fault generated by given access instruction in
 * bus_peek_<foo> or bus_poke_<foo> bus function.
 */
extern uint32_t generic_bs_peek_1f, generic_bs_peek_2f;
extern uint32_t generic_bs_peek_4f, generic_bs_peek_8f;
extern uint32_t generic_bs_poke_1f, generic_bs_poke_2f;
extern uint32_t generic_bs_poke_4f, generic_bs_poke_8f;

static bool
test_bs_fault(void *addr)
{
	return (addr == &generic_bs_peek_1f ||
	    addr == &generic_bs_peek_2f ||
	    addr == &generic_bs_peek_4f ||
	    addr == &generic_bs_peek_8f ||
	    addr == &generic_bs_poke_1f ||
	    addr == &generic_bs_poke_2f ||
	    addr == &generic_bs_poke_4f ||
	    addr == &generic_bs_poke_8f);
}

static void
svc_handler(struct thread *td, struct trapframe *frame)
{

	if ((frame->tf_esr & ESR_ELx_ISS_MASK) == 0) {
		syscallenter(td);
		syscallret(td);
	} else {
		call_trapsignal(td, SIGILL, ILL_ILLOPN, (void *)frame->tf_elr,
		    ESR_ELx_EXCEPTION(frame->tf_esr));
		userret(td, frame);
	}
}

static void
align_abort(struct thread *td, struct trapframe *frame, uint64_t esr,
    uint64_t far, int lower)
{
	if (!lower) {
		print_registers(frame);
		printf(" far: %16lx\n", far);
		printf(" esr:         %.8lx\n", esr);
		panic("Misaligned access from kernel space!");
	}

	call_trapsignal(td, SIGBUS, BUS_ADRALN, (void *)frame->tf_elr,
	    ESR_ELx_EXCEPTION(frame->tf_esr));
	userret(td, frame);
}


static void
external_abort(struct thread *td, struct trapframe *frame, uint64_t esr,
    uint64_t far, int lower)
{

	/*
	 * Try to handle synchronous external aborts caused by
	 * bus_space_peek() and/or bus_space_poke() functions.
	 */
	if (!lower && test_bs_fault((void *)frame->tf_elr)) {
		frame->tf_elr = (uint64_t)generic_bs_fault;
		return;
	}

	print_registers(frame);
	printf(" far: %16lx\n", far);
	panic("Unhandled EL%d external data abort", lower ? 0: 1);
}

static void
data_abort(struct thread *td, struct trapframe *frame, uint64_t esr,
    uint64_t far, int lower)
{
	struct vm_map *map;
	struct proc *p;
	struct pcb *pcb;
	vm_prot_t ftype;
	int error, sig, ucode;
#ifdef KDB
	bool handled;
#endif

	/*
	 * According to the ARMv8-A rev. A.g, B2.10.5 "Load-Exclusive
	 * and Store-Exclusive instruction usage restrictions", state
	 * of the exclusive monitors after data abort exception is unknown.
	 */
	clrex();

#ifdef KDB
	if (kdb_active) {
		kdb_reenter();
		return;
	}
#endif

	pcb = td->td_pcb;
	p = td->td_proc;
	if (lower)
		map = &p->p_vmspace->vm_map;
	else {
		intr_enable();

		/* The top bit tells us which range to use */
		if (far >= VM_MAXUSER_ADDRESS) {
			map = kernel_map;
		} else {
			map = &p->p_vmspace->vm_map;
			if (map == NULL)
				map = kernel_map;
		}
	}

	/*
	 * Try to handle translation, access flag, and permission faults.
	 * Translation faults may occur as a result of the required
	 * break-before-make sequence used when promoting or demoting
	 * superpages.  Such faults must not occur while holding the pmap lock,
	 * or pmap_fault() will recurse on that lock.
	 */
	if ((lower || map == kernel_map || pcb->pcb_onfault != 0) &&
	    pmap_fault(map->pmap, esr, far) == KERN_SUCCESS)
		return;

	KASSERT(td->td_md.md_spinlock_count == 0,
	    ("data abort with spinlock held"));
	if (td->td_critnest != 0 || WITNESS_CHECK(WARN_SLEEPOK |
	    WARN_GIANTOK, NULL, "Kernel page fault") != 0) {
		print_registers(frame);
		printf(" far: %16lx\n", far);
		printf(" esr:         %.8lx\n", esr);
		panic("data abort in critical section or under mutex");
	}

	switch (ESR_ELx_EXCEPTION(esr)) {
	case EXCP_INSN_ABORT:
	case EXCP_INSN_ABORT_L:
		ftype = VM_PROT_EXECUTE;
		break;
	default:
		ftype = (esr & ISS_DATA_WnR) == 0 ? VM_PROT_READ :
		    VM_PROT_WRITE;
		break;
	}

	/* Fault in the page. */
	error = vm_fault_trap(map, far, ftype, VM_FAULT_NORMAL, &sig, &ucode);
	if (error != KERN_SUCCESS) {
		if (lower) {
			call_trapsignal(td, sig, ucode, (void *)far,
			    ESR_ELx_EXCEPTION(esr));
		} else {
			if (td->td_intr_nesting_level == 0 &&
			    pcb->pcb_onfault != 0) {
				frame->tf_x[0] = error;
				frame->tf_elr = pcb->pcb_onfault;
				return;
			}

			printf("Fatal data abort:\n");
			print_registers(frame);
			printf(" far: %16lx\n", far);
			printf(" esr:         %.8lx\n", esr);

#ifdef KDB
			if (debugger_on_trap) {
				kdb_why = KDB_WHY_TRAP;
				handled = kdb_trap(ESR_ELx_EXCEPTION(esr), 0,
				    frame);
				kdb_why = KDB_WHY_UNSET;
				if (handled)
					return;
			}
#endif
			panic("vm_fault failed: %lx", frame->tf_elr);
		}
	}

	if (lower)
		userret(td, frame);
}

static void
print_registers(struct trapframe *frame)
{
	u_int reg;

	for (reg = 0; reg < nitems(frame->tf_x); reg++) {
		printf(" %sx%d: %16lx\n", (reg < 10) ? " " : "", reg,
		    frame->tf_x[reg]);
	}
	printf("  sp: %16lx\n", frame->tf_sp);
	printf("  lr: %16lx\n", frame->tf_lr);
	printf(" elr: %16lx\n", frame->tf_elr);
	printf("spsr:         %8x\n", frame->tf_spsr);
}

void
do_el1h_sync(struct thread *td, struct trapframe *frame)
{
	uint32_t exception;
	uint64_t esr, far;
	int dfsc;

	/* Read the esr register to get the exception details */
	esr = frame->tf_esr;
	exception = ESR_ELx_EXCEPTION(esr);

#ifdef KDTRACE_HOOKS
	if (dtrace_trap_func != NULL && (*dtrace_trap_func)(frame, exception))
		return;
#endif

	CTR4(KTR_TRAP,
	    "do_el1_sync: curthread: %p, esr %lx, elr: %lx, frame: %p", td,
	    esr, frame->tf_elr, frame);

	switch (exception) {
	case EXCP_FP_SIMD:
	case EXCP_TRAP_FP:
#ifdef VFP
		if ((td->td_pcb->pcb_fpflags & PCB_FP_KERN) != 0) {
			vfp_restore_state();
		} else
#endif
		{
			print_registers(frame);
			printf(" esr:         %.8lx\n", esr);
			panic("VFP exception in the kernel");
		}
		break;
	case EXCP_INSN_ABORT:
	case EXCP_DATA_ABORT:
		far = READ_SPECIALREG(far_el1);
		dfsc = esr & ISS_DATA_DFSC_MASK;
		if (dfsc < nitems(abort_handlers) &&
		    abort_handlers[dfsc] != NULL) {
			abort_handlers[dfsc](td, frame, esr, far, 0);
		} else {
			print_registers(frame);
			printf(" far: %16lx\n", far);
			printf(" esr:         %.8lx\n", esr);
			panic("Unhandled EL1 %s abort: %x",
			    exception == EXCP_INSN_ABORT ? "instruction" :
			    "data", dfsc);
		}
		break;
	case EXCP_BRK:
#ifdef KDTRACE_HOOKS
		if ((esr & ESR_ELx_ISS_MASK) == 0x40d && \
		    dtrace_invop_jump_addr != 0) {
			dtrace_invop_jump_addr(frame);
			break;
		}
#endif
#ifdef KDB
		kdb_trap(exception, 0, frame);
#else
		panic("No debugger in kernel.\n");
#endif
		break;
	case EXCP_WATCHPT_EL1:
	case EXCP_SOFTSTP_EL1:
#ifdef KDB
		kdb_trap(exception, 0, frame);
#else
		panic("No debugger in kernel.\n");
#endif
		break;
	case EXCP_UNKNOWN:
		if (undef_insn(1, frame))
			break;
		/* FALLTHROUGH */
	default:
		print_registers(frame);
		printf(" far: %16lx\n", READ_SPECIALREG(far_el1));
		panic("Unknown kernel exception %x esr_el1 %lx\n", exception,
		    esr);
	}
}

void
do_el0_sync(struct thread *td, struct trapframe *frame)
{
	pcpu_bp_harden bp_harden;
	uint32_t exception;
	uint64_t esr, far;
	int dfsc;

	/* Check we have a sane environment when entering from userland */
	KASSERT((uintptr_t)get_pcpu() >= VM_MIN_KERNEL_ADDRESS,
	    ("Invalid pcpu address from userland: %p (tpidr %lx)",
	     get_pcpu(), READ_SPECIALREG(tpidr_el1)));

	esr = frame->tf_esr;
	exception = ESR_ELx_EXCEPTION(esr);
	switch (exception) {
	case EXCP_INSN_ABORT_L:
		far = READ_SPECIALREG(far_el1);

		/*
		 * Userspace may be trying to train the branch predictor to
		 * attack the kernel. If we are on a CPU affected by this
		 * call the handler to clear the branch predictor state.
		 */
		if (far > VM_MAXUSER_ADDRESS) {
			bp_harden = PCPU_GET(bp_harden);
			if (bp_harden != NULL)
				bp_harden();
		}
		break;
	case EXCP_UNKNOWN:
	case EXCP_DATA_ABORT_L:
	case EXCP_DATA_ABORT:
	case EXCP_WATCHPT_EL0:
		far = READ_SPECIALREG(far_el1);
		break;
	}
	intr_enable();

	CTR4(KTR_TRAP,
	    "do_el0_sync: curthread: %p, esr %lx, elr: %lx, frame: %p", td, esr,
	    frame->tf_elr, frame);

	switch (exception) {
	case EXCP_FP_SIMD:
	case EXCP_TRAP_FP:
#ifdef VFP
		vfp_restore_state();
#else
		panic("VFP exception in userland");
#endif
		break;
	case EXCP_SVC32:
	case EXCP_SVC64:
		svc_handler(td, frame);
		break;
	case EXCP_INSN_ABORT_L:
	case EXCP_DATA_ABORT_L:
	case EXCP_DATA_ABORT:
		dfsc = esr & ISS_DATA_DFSC_MASK;
		if (dfsc < nitems(abort_handlers) &&
		    abort_handlers[dfsc] != NULL)
			abort_handlers[dfsc](td, frame, esr, far, 1);
		else {
			print_registers(frame);
			printf(" far: %16lx\n", far);
			printf(" esr:         %.8lx\n", esr);
			panic("Unhandled EL0 %s abort: %x",
			    exception == EXCP_INSN_ABORT_L ? "instruction" :
			    "data", dfsc);
		}
		break;
	case EXCP_UNKNOWN:
		if (!undef_insn(0, frame))
			call_trapsignal(td, SIGILL, ILL_ILLTRP, (void *)far,
			    exception);
		userret(td, frame);
		break;
	case EXCP_SP_ALIGN:
		call_trapsignal(td, SIGBUS, BUS_ADRALN, (void *)frame->tf_sp,
		    exception);
		userret(td, frame);
		break;
	case EXCP_PC_ALIGN:
		call_trapsignal(td, SIGBUS, BUS_ADRALN, (void *)frame->tf_elr,
		    exception);
		userret(td, frame);
		break;
	case EXCP_BRKPT_EL0:
	case EXCP_BRK:
		call_trapsignal(td, SIGTRAP, TRAP_BRKPT, (void *)frame->tf_elr,
		    exception);
		userret(td, frame);
		break;
	case EXCP_WATCHPT_EL0:
		call_trapsignal(td, SIGTRAP, TRAP_TRACE, (void *)far,
		    exception);
		userret(td, frame);
		break;
	case EXCP_MSR:
		/*
		 * The CPU can raise EXCP_MSR when userspace executes an mrs
		 * instruction to access a special register userspace doesn't
		 * have access to.
		 */
		if (!undef_insn(0, frame))
			call_trapsignal(td, SIGILL, ILL_PRVOPC,
			    (void *)frame->tf_elr, exception);
		userret(td, frame);
		break;
	case EXCP_SOFTSTP_EL0:
		td->td_frame->tf_spsr &= ~PSR_SS;
		td->td_pcb->pcb_flags &= ~PCB_SINGLE_STEP;
		WRITE_SPECIALREG(mdscr_el1,
		    READ_SPECIALREG(mdscr_el1) & ~DBG_MDSCR_SS);
		call_trapsignal(td, SIGTRAP, TRAP_TRACE,
		    (void *)frame->tf_elr, exception);
		userret(td, frame);
		break;
	default:
		call_trapsignal(td, SIGBUS, BUS_OBJERR, (void *)frame->tf_elr,
		    exception);
		userret(td, frame);
		break;
	}

	KASSERT((td->td_pcb->pcb_fpflags & ~PCB_FP_USERMASK) == 0,
	    ("Kernel VFP flags set while entering userspace"));
	KASSERT(
	    td->td_pcb->pcb_fpusaved == &td->td_pcb->pcb_fpustate,
	    ("Kernel VFP state in use when entering userspace"));
}

/*
 * TODO: We will need to handle these later when we support ARMv8.2 RAS.
 */
void
do_serror(struct trapframe *frame)
{
	uint64_t esr, far;

	far = READ_SPECIALREG(far_el1);
	esr = frame->tf_esr;

	print_registers(frame);
	printf(" far: %16lx\n", far);
	printf(" esr:         %.8lx\n", esr);
	panic("Unhandled System Error");
}

void
unhandled_exception(struct trapframe *frame)
{
	uint64_t esr, far;

	far = READ_SPECIALREG(far_el1);
	esr = frame->tf_esr;

	print_registers(frame);
	printf(" far: %16lx\n", far);
	printf(" esr:         %.8lx\n", esr);
	panic("Unhandled exception");
}