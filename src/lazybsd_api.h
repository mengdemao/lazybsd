/**
 * @file lazybsd_api.h
 * @author mengdemao (mengdemao19951021@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __LAZYBSD_API_H__
#define __LAZYBSD_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <sys/time.h>

#include "lazybsd_event.h"
#include "lazybsd_errno.h"

struct linux_sockaddr {
    short sa_family;
    char sa_data[14];
};

#define AF_INET6_LINUX          10
#define PF_INET6_LINUX          AF_INET6_LINUX
#define AF_INET6_FREEBSD        28
#define PF_INET6_FREEBSD        AF_INET6_FREEBSD

typedef int (*loop_func_t)(void *arg);

/* route api begin */
enum LAZYBSD_ROUTE_CTL {
    LAZYBSD_ROUTE_ADD,
    LAZYBSD_ROUTE_DEL,
    LAZYBSD_ROUTE_CHANGE,
};

enum LAZYBSD_ROUTE_FLAG {
    LAZYBSD_RTF_HOST,
    LAZYBSD_RTF_GATEWAY,
};

/* dispatch api begin */
#define LAZYBSD_DISPATCH_ERROR (-1)
#define LAZYBSD_DISPATCH_RESPONSE (-2)

/*
 * Packet dispatch callback function.
 * Implemented by user.
 *
 * @param data
 *   The data pointer of this packet.
 * @param len
 *   The length of this packet.
 * @param queue_id
 *   Current queue of this packet.
 * @param nb_queues
 *   Number of queues to be dispatched.
 *
 * @return 0 to (nb_queues - 1)
 *   The queue id that the packet will be dispatched to.
 * @return LAZYBSD_DISPATCH_ERROR (-1)
 *   Error occurs or packet is handled by user, packet will be freed.
* @return LAZYBSD_DISPATCH_RESPONSE (-2)
 *   Packet is handled by user, packet will be responsed.
 *
 */
typedef int (*dispatch_func_t)(void *data, uint16_t *len,
    uint16_t queue_id, uint16_t nb_queues);

/* pcb lddr api begin */
/*
 * pcb lddr callback function.
 * Implemented by user.
 *
 * @param family
 *   The remote server addr, should be AF_INET or AF_INET6.
 * @param dst_addr
 *   The remote server addr, should be (in_addr *) or (in6_addr *).
 * @param dst_port
 *   The remote server port.
 * @param src_addr
 *   Return parameter.
 *   The local addr, should be (in_addr *) or (in6_addr *).
 *   If set (INADDR_ANY) or (in6addr_any), the app then will
 *   call `in_pcbladdr()` to get laddr.
 *
 * @return error_no
 *   0 means success.
 *
 */
typedef int (*pcblddr_func_t)(uint16_t family, void *dst_addr,
    uint16_t dst_port, void *src_addr);

/* regist a pcb lddr function */
void lazybsd_regist_pcblddr_fun(pcblddr_func_t func);

/* pcb lddr api end */

/* internal api begin */

/* FreeBSD style calls. Used for tools. */
int lazybsd_ioctl_freebsd(int fd, unsigned long request, ...);
int lazybsd_setsockopt_freebsd(int s, int level, int optname,
    const void *optval, socklen_t optlen);
int lazybsd_getsockopt_freebsd(int s, int level, int optname,
    void *optval, socklen_t *optlen);

/*
 * Handle rtctl.
 * The data is a pointer to struct rt_msghdr.
 */
int lazybsd_rtioctl(int fib, void *data, unsigned *plen, unsigned maxlen);

/*
 * Handle ngctl.
 */
enum LAZYBSD_NGCTL_CMD {
    NGCTL_SOCKET,
    NGCTL_BIND,
    NGCTL_CONNECT,
    NGCTL_SEND,
    NGCTL_RECV,
    NGCTL_CLOSE,
};

int lazybsd_ngctl(int cmd, void *data);

/* internal api end */

/* zero ccopy API begin */
struct lazybsd_zc_mbuf {
    void *bsd_mbuf;         /* point to the head mbuf */
    void *bsd_mbuf_off;     /* ponit to the current mbuf in the mbuf chain with offset */
    int off;                /* the offset of total mbuf, APP shouldn't modify it */
    int len;                /* the total len of the mbuf chain */
};

/*
 * Get the ff zero copy mbuf.
 *
 * @param m
 *   The ponitor of 'sturct lazybsd_zc_mbuf', and can't be NULL.
 *   Can used by 'lazybsd_zc_mbuf_write' and 'lazybsd_zc_mbuf_read'.
 * @param len
 *   The total buf len of mbuf chain that you want to alloc.
 *
 * @return error_no
 *   0 means success.
 *  -1 means error.
 */
int lazybsd_zc_mbuf_get(struct lazybsd_zc_mbuf *m, int len);

/*
 * Write data to the mbuf chain in 'sturct lazybsd_zc_mbuf'.
 * APP can call this function multiple times, need pay attion to the offset of data.
 * but the total len can't be larger than m->len.
 * After this fuction return success,
 *
 * the struct 'lazybsd_zc_mbuf *m' can be reused in `lazybsd_zc_mbuf_get` and then Use normally.
 * Nerver directly reused in `lazybsd_zc_mbuf_write` before recall `lazybsd_zc_mbuf_get`.
 *
 * APP nedd call 'lazybsd_write' to send data actually after finish write data to mbuf,
 * And use 'bsd_mbuf' of 'struct lazybsd_zc_mbuf' as the 'buf' argument.
 *
 * See 'example/main_zc.c'
 *
 * @param m
 *   The ponitor of 'sturct lazybsd_zc_mbuf', must be call 'lazybsd_zc_mbuf_get' first.
 * @param data
 *   The pointer of data that want to write to socket, need pay attion to the offset.
 * @param len
 *   The len that APP want to write to mbuf chain this time.
 *
 * @return error_no
 *   0 means success.
 *  -1 means error.
 */
int lazybsd_zc_mbuf_write(struct lazybsd_zc_mbuf *m, const char *data, int len);

/*
 * Read data to the mbuf chain in 'sturct lazybsd_zc_mbuf'.
 * not implemented now.
 */
int lazybsd_zc_mbuf_read(struct lazybsd_zc_mbuf *m, const char *data, int len);

/* ZERO COPY API end */

#ifdef __cplusplus
}
#endif
#endif // __LAZYBSD_API_H__

