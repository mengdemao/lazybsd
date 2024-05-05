#ifndef _FSTACK_API_H
#define _FSTACK_API_H

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

#define AF_INET6_LINUX    10
#define PF_INET6_LINUX    AF_INET6_LINUX
#define AF_INET6_FREEBSD    28
#define PF_INET6_FREEBSD    AF_INET6_FREEBSD

typedef int (*loop_func_t)(void *arg);

int lazybsd_init(int argc, char * const argv[]);

void lazybsd_run(loop_func_t loop, void *arg);

/* POSIX-LIKE api begin */

int lazybsd_fcntl(int fd, int cmd, ...);

int lazybsd_sysctl(const int *name, u_int namelen, void *oldp, size_t *oldlenp,
    const void *newp, size_t newlen);

int lazybsd_ioctl(int fd, unsigned long request, ...);

/*
 * While get sockfd from this API, and then need set it to non-blocking mode like this,
 * Otherwise, sometimes the socket interface will not work properly, such as `lazybsd_write()`
 *
 *    int on = 1;
 *    lazybsd_ioctl(sockfd, FIONBIO, &on);
 *
 *  See also `example/main.c`
 */
int lazybsd_socket(int domain, int type, int protocol);

int lazybsd_setsockopt(int s, int level, int optname, const void *optval,
    socklen_t optlen);

int lazybsd_getsockopt(int s, int level, int optname, void *optval,
    socklen_t *optlen);

int lazybsd_listen(int s, int backlog);
int lazybsd_bind(int s, const struct linux_sockaddr *addr, socklen_t addrlen);
int lazybsd_accept(int s, struct linux_sockaddr *addr, socklen_t *addrlen);
int lazybsd_connect(int s, const struct linux_sockaddr *name, socklen_t namelen);
int lazybsd_close(int fd);
int lazybsd_shutdown(int s, int how);

int lazybsd_getpeername(int s, struct linux_sockaddr *name,
    socklen_t *namelen);
int lazybsd_getsockname(int s, struct linux_sockaddr *name,
    socklen_t *namelen);

ssize_t lazybsd_read(int d, void *buf, size_t nbytes);
ssize_t lazybsd_readv(int fd, const struct iovec *iov, int iovcnt);


/*
 * Write data to the socket sendspace buf.
 *
 * Note:
 * The `fd` parameter need set non-blocking mode in advance if F-Stack's APP.
 * Otherwise if the `nbytes` parameter is greater than
 * `net.inet.tcp.sendspace + net.inet.tcp.sendbuf_inc`,
 * the API will return -1, but not the length that has been sent.
 *
 * You also can modify the value of  `net.inet.tcp.sendspace`(default 16384 bytes)
 * and `net.inet.tcp.sendbuf_inc`(default 16384 bytes) with `config.ini`.
 * But it should be noted that not all parameters can take effect, such as 32768 and 32768.
 * `lazybsd_sysctl` can see there values while APP is running.
 */
ssize_t lazybsd_write(int fd, const void *buf, size_t nbytes);
ssize_t lazybsd_writev(int fd, const struct iovec *iov, int iovcnt);

ssize_t lazybsd_send(int s, const void *buf, size_t len, int flags);
ssize_t lazybsd_sendto(int s, const void *buf, size_t len, int flags,
    const struct linux_sockaddr *to, socklen_t tolen);
ssize_t lazybsd_sendmsg(int s, const struct msghdr *msg, int flags);

ssize_t lazybsd_recv(int s, void *buf, size_t len, int flags);
ssize_t lazybsd_recvfrom(int s, void *buf, size_t len, int flags,
    struct linux_sockaddr *from, socklen_t *fromlen);
ssize_t lazybsd_recvmsg(int s, struct msghdr *msg, int flags);

int lazybsd_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
    struct timeval *timeout);

int lazybsd_poll(struct pollfd fds[], nfds_t nfds, int timeout);

int lazybsd_kqueue(void);
int lazybsd_kevent(int kq, const struct kevent *changelist, int nchanges,
    struct kevent *eventlist, int nevents, const struct timespec *timeout);
int lazybsd_kevent_do_each(int kq, const struct kevent *changelist, int nchanges,
    void *eventlist, int nevents, const struct timespec *timeout,
    void (*do_each)(void **, struct kevent *));

#if __GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 31)
int lazybsd_gettimeofday(struct timeval *tv, void *tz);
#else
int lazybsd_gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

int lazybsd_dup(int oldfd);
int lazybsd_dup2(int oldfd, int newfd);

/* POSIX-LIKE api end */


/* Tests if fd is used by F-Stack */
extern int lazybsd_fdisused(int fd);

extern int lazybsd_getmaxfd(void);

/*
 * Get traffic for QoS or other via API.
 * The size of buffer must >= siezof(struct lazybsd_traffic_args), now is 32 bytes.
 */
void lazybsd_get_traffic(void *buffer);

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

/*
 * On success, 0 is returned.
 * On error, -1 is returned, and errno is set appropriately.
 */
int lazybsd_route_ctl(enum LAZYBSD_ROUTE_CTL req, enum LAZYBSD_ROUTE_FLAG flag,
    struct linux_sockaddr *dst, struct linux_sockaddr *gw,
    struct linux_sockaddr *netmask);

/* route api end */


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

/* regist a packet dispath function */
void lazybsd_regist_packet_dispatcher(dispatch_func_t func);

/* dispatch api end */

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
#endif

