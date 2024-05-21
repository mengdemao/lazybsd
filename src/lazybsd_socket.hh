/**
 * @file lazybsd_socket.hh
 * @author mengdemao (mengdemao19951021@gmail.com)
 * @brief lazybsd socket wrapper
 * @version 0.1
 * @date 2024-05-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <cstddef>
#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>
#include "lazybsd_api.h"
#include "lazybsd_socket.h"

#ifndef LAZYBSD_SOCKET_HXX
#define LAZYBSD_SOCKET_HXX

#define AF_INET6_LINUX    10
#define PF_INET6_LINUX    AF_INET6_LINUX
#define AF_INET6_FREEBSD    28
#define PF_INET6_FREEBSD    AF_INET6_FREEBSD

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

int lazybsd_gettimeofday(struct timeval *tv, void *tz);

int lazybsd_dup(int oldfd);
int lazybsd_dup2(int oldfd, int newfd);

#endif // LAZYBSD_SOCKET_HXX