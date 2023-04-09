cmake_minimum_required(VERSION 3.24)

project(bsd C CXX ASM)

set(src_crypto 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/crypto/sha1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/crypto/siphash/siphash.c
)

set(src_kern
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/kern_descrip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/kern_event.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/kern_fail.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/kern_khelp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/kern_hhook.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/kern_linker.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/kern_mbuf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/kern_module.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/kern_mtxpool.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/kern_ntptime.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/kern_osd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/kern_sysctl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/kern_tc.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/kern_uuid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/link_elf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/md5c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/subr_capability.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/subr_counter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/subr_eventhandler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/subr_kobj.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/subr_lock.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/subr_module.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/subr_param.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/subr_pcpu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/subr_sbuf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/subr_taskqueue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/subr_unit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/subr_smr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/sys_capability.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/sys_generic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/sys_socket.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/uipc_accf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/uipc_mbuf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/uipc_mbuf2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/uipc_domain.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/uipc_sockbuf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/uipc_socket.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/uipc_syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/subr_filter.c
)

set(src_libkern
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/amd64/amd64/in_cksum.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/libkern/bcd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/libkern/gsb_crc32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/libkern/inet_ntoa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/libkern/jenkins_hash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/libkern/strlcpy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/libkern/strnlen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/libkern/arc4random_uniform.c
)
set(src_net
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/bpf.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/bridgestp.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/if.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/if_bridge.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/if_clone.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/if_dead.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/if_ethersubr.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/if_loop.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/if_llatbl.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/if_media.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/if_spppfr.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/if_spppsubr.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/if_vlan.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/if_vxlan.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/netisr.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/pfil.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/radix.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/raw_cb.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/raw_usrreq.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/route.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/route/route_ctl.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/route/route_tables.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/route/route_helpers.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/route/route_ifaddrs.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/route/route_temporal.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/route/nhop_utils.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/route/nhop.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/route/nhop_ctl.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/rtsock.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/slcompress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/net/if_gif.c 
)

set(src_netinet
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/in_fib.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/in_gif.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/ip_reass.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/if_ether.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/igmp.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/in.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/in_mcast.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/in_pcb.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/in_proto.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/in_rmx.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/ip_carp.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/ip_divert.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/ip_ecn.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/ip_encap.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/ip_fastfwd.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/ip_icmp.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/ip_id.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/ip_input.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/ip_mroute.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/ip_options.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/ip_output.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/raw_ip.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_debug.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_hostcache.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_input.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_lro.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_offload.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_output.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_reass.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_sack.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_subr.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_syncache.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_timer.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_timewait.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_usrreq.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/udp_usrreq.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/cc/cc.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/cc/cc_newreno.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/cc/cc_htcp.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/cc/cc_cubic.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/libalias/alias.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/libalias/alias_db.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/libalias/alias_mod.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/libalias/alias_proxy.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/libalias/alias_sctp.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/libalias/alias_util.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_hpts.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_ratelimit.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_stacks/sack_filter.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_stacks/rack_bbr_common.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_stacks/rack.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet/tcp_stacks/bbr.c
)

set(src_netinet6 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/dest6.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/frag6.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/icmp6.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/in6.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/in6_ifattach.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/in6_mcast.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/in6_pcb.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/in6_pcbgroup.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/in6_proto.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/in6_rmx.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/in6_src.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/ip6_forward.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/ip6_id.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/ip6_input.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/ip6_fastfwd.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/ip6_mroute.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/ip6_output.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/mld6.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/nd6.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/nd6_nbr.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/nd6_rtr.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/raw_ip6.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/route6.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/scope6.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/send.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/udp6_usrreq.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/in6_cksum.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/in6_fib.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6/in6_gif.c 
)

set(src_vm
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/vm/uma_core.c 
)

add_library(${PROJECT_NAME} STATIC
    ${src_vm}  ${src_net} ${src_kern} ${src_crypto} 
    ${src_libkern}  ${src_netinet}  ${src_netinet6}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wno-format-truncation
    -Wnested-externs 
    -Wstrict-prototypes 
    -Wmissing-prototypes 
    -Wpointer-arith 
    -Wno-inline
    -Wno-pointer-sign 
    -Wmissing-include-dirs 
    -Wno-unused-but-set-variable 
    -Wno-address-of-packed-member
    -nostdinc
    -finline-limit=8000 
    -fdiagnostics-show-option
    -fno-builtin
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wno-cast-qual
    -Wno-stringop-overread
    -Wno-stringop-overflow
    -Wno-format
    -Wno-dangling-pointer
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    -DLAZYBSD=1
    -DINET 
    -DINET6 
    -DTCPHPTS 
    -D__FreeBSD__ 
    -D_KERNEL 
    -DHAVE_KERNEL_OPTION_HEADERS
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/amd64/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/sys/contrib/ck/include
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/opt
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD 
    COMMAND awk -f ${CMAKE_CURRENT_SOURCE_DIR}/sys/tools/makeobjops.awk ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/bus_if.m -h
    COMMAND awk -f ${CMAKE_CURRENT_SOURCE_DIR}/sys/tools/makeobjops.awk ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/device_if.m -h
    COMMAND awk -f ${CMAKE_CURRENT_SOURCE_DIR}/sys/tools/makeobjops.awk ${CMAKE_CURRENT_SOURCE_DIR}/sys/opencrypto/cryptodev_if.m -h
    COMMAND awk -f ${CMAKE_CURRENT_SOURCE_DIR}/sys/tools/makeobjops.awk ${CMAKE_CURRENT_SOURCE_DIR}/sys/opencrypto/cryptodev_if.m -c
    COMMAND awk -f ${CMAKE_CURRENT_SOURCE_DIR}/sys/tools/makeobjops.awk ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/linker_if.m -h
    COMMAND awk -f ${CMAKE_CURRENT_SOURCE_DIR}/sys/tools/makeobjops.awk ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/linker_if.m -c
    COMMAND awk -f ${CMAKE_CURRENT_SOURCE_DIR}/sys/tools/vnode_if.awk   ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/vnode_if.src -p
    COMMAND awk -f ${CMAKE_CURRENT_SOURCE_DIR}/sys/tools/vnode_if.awk   ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/vnode_if.src -q
    COMMAND awk -f ${CMAKE_CURRENT_SOURCE_DIR}/sys/tools/vnode_if.awk   ${CMAKE_CURRENT_SOURCE_DIR}/sys/kern/vnode_if.src -h               
)
