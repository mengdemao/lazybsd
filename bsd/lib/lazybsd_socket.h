/**
 * @file lazybsd_socket.h
 * @author Meng Demao (mengdemao19951021@163.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <poll.h>

#ifndef __LAZYBSD_SOCKET_H__
#define __LAZYBSD_SOCKET_H__

#ifdef __cplusplus
extern "C" {
#endif

struct linux_sockaddr {
    short sa_family;
    char sa_data[14];
};

#define AF_INET6_LINUX      10
#define PF_INET6_LINUX      AF_INET6_LINUX
#define AF_INET6_FREEBSD    28
#define PF_INET6_FREEBSD    AF_INET6_FREEBSD

#define LINUX_AF_INET6      10

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

int lazybsd_gettimeofday(struct timeval *tv, struct timezone *tz);

int lazybsd_dup(int oldfd);
int lazybsd_dup2(int oldfd, int newfd);

#ifdef __cplusplus
}
#endif

#endif /* __LAZYBSD_SOCKET_H__ */