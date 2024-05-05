/**
 * @file lazybsd_host.cc
 * @author mengdemao (mengdemao19951021@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>

#include <openssl/rand.h>
#include <rte_malloc.h>

#include "lazybsd_host.hh"
#include "lazybsd_errno.hh"

static struct timespec current_ts;
extern void* lazybsd_mem_get_page();
extern int lazybsd_mem_free_addr(void* p);

void *
lazybsd_mmap(void *addr, uint64_t len, int prot, int flags, int fd, uint64_t offset)
{
    //return rte_malloc("", len, 4096);
    int host_prot;
    int host_flags;

#ifdef LAZYBSD_USE_PAGE_ARRAY
        if( len == 4096 ){
            return lazybsd_mem_get_page();
        }
        else
#endif
        {

    assert(lazybsd_PROT_NONE == PROT_NONE);
    host_prot = 0;
    if ((prot & lazybsd_PROT_READ) == lazybsd_PROT_READ)   host_prot |= PROT_READ;
    if ((prot & lazybsd_PROT_WRITE) == lazybsd_PROT_WRITE) host_prot |= PROT_WRITE;

    host_flags = 0;
    if ((flags & lazybsd_MAP_SHARED) == lazybsd_MAP_SHARED)   host_flags |= MAP_SHARED;
    if ((flags & lazybsd_MAP_PRIVATE) == lazybsd_MAP_PRIVATE) host_flags |= MAP_PRIVATE;
    if ((flags & lazybsd_MAP_ANON) == lazybsd_MAP_ANON)       host_flags |= MAP_ANON;

    void *ret = (mmap(addr, len, host_prot, host_flags, fd, offset));

    if ((uint64_t)ret == -1) {
        printf("fst mmap failed:%s\n", strerror(errno));
        exit(1);
    }
    return ret;
    }
}

int
lazybsd_munmap(void *addr, uint64_t len)
{
#ifdef LAZYBSD_USE_PAGE_ARRAY
        if ( len == 4096 ){
            return lazybsd_mem_free_addr(addr);
        }
#endif
    //rte_free(addr);
    //return 0;
    return (munmap(addr, len));
}


void *
lazybsd_malloc(uint64_t size)
{
    //return rte_malloc("", size, 0);
    return (malloc(size));
}


void *
lazybsd_calloc(uint64_t number, uint64_t size)
{
    //return rte_calloc("", number, size, 0);
    return (calloc(number, size));
}


void *
lazybsd_realloc(void *p, uint64_t size)
{
    if (size) {
        //return rte_realloc(p, size, 0);
        return (realloc(p, size));
    }

    return (p);
}


void
lazybsd_free(void *p)
{
    //rte_free(p);
    free(p);
}

void panic(const char *, ...) __attribute__((__noreturn__));

const char *panicstr = NULL;

void
panic(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    abort();
}

void
lazybsd_clock_gettime(int id, int64_t *sec, long *nsec)
{
    struct timespec ts;
    int host_id;
    int rv;

    switch (id) {
    case lazybsd_CLOCK_REALTIME:
        host_id = CLOCK_REALTIME;
        break;
#ifdef CLOCK_MONOTONIC_FAST
    case lazybsd_CLOCK_MONOTONIC_FAST:
        host_id = CLOCK_MONOTONIC_FAST;
        break;
#endif
    case lazybsd_CLOCK_MONOTONIC:
    default:
        host_id = CLOCK_MONOTONIC;
        break;
    }

    rv = clock_gettime(host_id, &ts);
    assert(0 == rv);

    *sec = (int64_t)ts.tv_sec;
    *nsec = (long)ts.tv_nsec;
}

uint64_t
lazybsd_clock_gettime_ns(int id)
{
    int64_t sec;
    long nsec;

    lazybsd_clock_gettime(id, &sec, &nsec);

    return ((uint64_t)sec * lazybsd_NSEC_PER_SEC + nsec);
}

void
lazybsd_get_current_time(time_t *sec, long *nsec)
{
    if (sec) {
        *sec = current_ts.tv_sec;
    }

    if (nsec) {
        *nsec = current_ts.tv_nsec;
    }
}

void
lazybsd_update_current_ts()
{
    int rv = clock_gettime(CLOCK_REALTIME, &current_ts);
    assert(rv == 0);
}

void
lazybsd_arc4rand(void *ptr, unsigned int len, int reseed)
{
    (void)reseed;

    RAND_bytes((unsigned char *)ptr, len);
}

uint32_t
lazybsd_arc4random(void)
{
    uint32_t ret;
    lazybsd_arc4rand(&ret, sizeof ret, 0);
    return ret;
}

int lazybsd_setenv(const char *name, const char *value)
{
    return setenv(name, value, 1);
}

char *lazybsd_getenv(const char *name)
{
    return getenv(name);
}

void lazybsd_os_errno(int error)
{
    switch (error) {
        case lazybsd_EPERM:       errno = EPERM; break;
        case lazybsd_ENOENT:      errno = ENOENT; break;
        case lazybsd_ESRCH:       errno = ESRCH; break;
        case lazybsd_EINTR:       errno = EINTR; break;
        case lazybsd_EIO:         errno = EIO; break;
        case lazybsd_ENXIO:       errno = ENXIO; break;
        case lazybsd_E2BIG:       errno = E2BIG; break;
        case lazybsd_ENOEXEC:     errno = ENOEXEC; break;
        case lazybsd_EBADF:       errno = EBADF; break;
        case lazybsd_ECHILD:      errno = ECHILD; break;
        case lazybsd_EDEADLK:     errno = EDEADLK; break;
        case lazybsd_ENOMEM:      errno = ENOMEM; break;
        case lazybsd_EACCES:      errno = EACCES; break;
        case lazybsd_EFAULT:      errno = EFAULT; break;
        case lazybsd_ENOTBLK:     errno = ENOTBLK; break;
        case lazybsd_EBUSY:       errno = EBUSY; break;
        case lazybsd_EEXIST:      errno = EEXIST; break;
        case lazybsd_EXDEV:       errno = EXDEV; break;
        case lazybsd_ENODEV:      errno = ENODEV; break;
        case lazybsd_ENOTDIR:     errno = ENOTDIR; break;
        case lazybsd_EISDIR:      errno = EISDIR; break;
        case lazybsd_EINVAL:      errno = EINVAL; break;
        case lazybsd_ENFILE:      errno = ENFILE; break;
        case lazybsd_EMFILE:      errno = EMFILE; break;
        case lazybsd_ENOTTY:      errno = ENOTTY; break;
        case lazybsd_ETXTBSY:     errno = ETXTBSY; break;
        case lazybsd_EFBIG:       errno = EFBIG; break;
        case lazybsd_ENOSPC:      errno = ENOSPC; break;
        case lazybsd_ESPIPE:      errno = ESPIPE; break;
        case lazybsd_EROFS:       errno = EROFS; break;
        case lazybsd_EMLINK:      errno = EMLINK; break;
        case lazybsd_EPIPE:       errno = EPIPE; break;
        case lazybsd_EDOM:        errno = EDOM; break;
        case lazybsd_ERANGE:      errno = ERANGE; break;
    
        /* case lazybsd_EAGAIN:       same as EWOULDBLOCK */
        case lazybsd_EWOULDBLOCK:     errno = EWOULDBLOCK; break;
    
        case lazybsd_EINPROGRESS:     errno = EINPROGRESS; break;
        case lazybsd_EALREADY:        errno = EALREADY; break;
        case lazybsd_ENOTSOCK:        errno = ENOTSOCK; break;
        case lazybsd_EDESTADDRREQ:    errno = EDESTADDRREQ; break;
        case lazybsd_EMSGSIZE:        errno = EMSGSIZE; break;
        case lazybsd_EPROTOTYPE:      errno = EPROTOTYPE; break;
        case lazybsd_ENOPROTOOPT:     errno = ENOPROTOOPT; break;
        case lazybsd_EPROTONOSUPPORT: errno = EPROTONOSUPPORT; break;
        case lazybsd_ESOCKTNOSUPPORT: errno = ESOCKTNOSUPPORT; break;

        /* case lazybsd_EOPNOTSUPP:   same as ENOTSUP */
        case lazybsd_ENOTSUP:         errno = ENOTSUP; break;

        case lazybsd_EPFNOSUPPORT:    errno = EPFNOSUPPORT; break;
        case lazybsd_EAFNOSUPPORT:    errno = EAFNOSUPPORT; break;
        case lazybsd_EADDRINUSE:      errno = EADDRINUSE; break;
        case lazybsd_EADDRNOTAVAIL:   errno = EADDRNOTAVAIL; break;
        case lazybsd_ENETDOWN:        errno = ENETDOWN; break;
        case lazybsd_ENETUNREACH:     errno = ENETUNREACH; break;
        case lazybsd_ENETRESET:       errno = ENETRESET; break;
        case lazybsd_ECONNABORTED:    errno = ECONNABORTED; break;
        case lazybsd_ECONNRESET:      errno = ECONNRESET; break;
        case lazybsd_ENOBUFS:         errno = ENOBUFS; break;
        case lazybsd_EISCONN:         errno = EISCONN; break;
        case lazybsd_ENOTCONN:        errno = ENOTCONN; break;
        case lazybsd_ESHUTDOWN:       errno = ESHUTDOWN; break;
        case lazybsd_ETOOMANYREFS:    errno = ETOOMANYREFS; break;
        case lazybsd_ETIMEDOUT:       errno = ETIMEDOUT; break;
        case lazybsd_ECONNREFUSED:    errno = ECONNREFUSED; break;
        case lazybsd_ELOOP:           errno = ELOOP; break;
        case lazybsd_ENAMETOOLONG:    errno = ENAMETOOLONG; break;
        case lazybsd_EHOSTDOWN:       errno = EHOSTDOWN; break;
        case lazybsd_EHOSTUNREACH:    errno = EHOSTUNREACH; break;
        case lazybsd_ENOTEMPTY:       errno = ENOTEMPTY; break;
        case lazybsd_EUSERS:      errno = EUSERS; break;
        case lazybsd_EDQUOT:      errno = EDQUOT; break;
        case lazybsd_ESTALE:      errno = ESTALE; break;
        case lazybsd_EREMOTE:     errno = EREMOTE; break;
        case lazybsd_ENOLCK:      errno = ENOLCK; break;
        case lazybsd_ENOSYS:      errno = ENOSYS; break;
        case lazybsd_EIDRM:       errno = EIDRM; break;
        case lazybsd_ENOMSG:      errno = ENOMSG; break;
        case lazybsd_EOVERFLOW:   errno = EOVERFLOW; break;
        case lazybsd_ECANCELED:   errno = ECANCELED; break;
        case lazybsd_EILSEQ:      errno = EILSEQ; break;
        case lazybsd_EBADMSG:     errno = EBADMSG; break;
        case lazybsd_EMULTIHOP:   errno = EMULTIHOP; break;
        case lazybsd_ENOLINK:     errno = ENOLINK; break;
        case lazybsd_EPROTO:      errno = EPROTO; break;
        default:              errno = error; break;
    }

}

