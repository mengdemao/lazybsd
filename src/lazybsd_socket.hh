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
#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>
#include "lazybsd_api.h"
#include "lazybsd_socket.h"

#ifndef LAZYBSD_SOCKET_HXX
#define LAZYBSD_SOCKET_HXX

namespace lazybsd {
namespace net {

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

int lazybsd_kqueue(void);
int lazybsd_kevent(int kq, const struct kevent *changelist, int nchanges,
    struct kevent *eventlist, int nevents, const struct timespec *timeout);
int lazybsd_kevent_do_each(int kq, const struct kevent *changelist, int nchanges,
    void *eventlist, int nevents, const struct timespec *timeout,
    void (*do_each)(void **, struct kevent *));

int lazybsd_gettimeofday(struct timeval *tv, void *tz);

int lazybsd_dup(int oldfd);
int lazybsd_dup2(int oldfd, int newfd);

} // net
} // lazybsd

#endif // LAZYBSD_SOCKET_HXX
