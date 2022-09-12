/**
 * @file bypass_sock.h
 * @author mengdemao (mengdemao19951021@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __BYPASS_SOCK_H__
#define __BYPASS_SOCK_H__

#include <sys/socket.h>

class bypass_sock:public bypass {
private:
    int fd;
    int domain;
    int type;
    int protocol;

public:
    bypass_sock(string device, int domain, int type, int protocol);
    ~bypass_sock();

    int bind(struct sockaddr *addr, socklen_t addrlen);
    int listen(int backlog);

    int connect(const struct sockaddr *addr, socklen_t addrlen);
    int accept(struct sockaddr *addr, socklen_t *addrlen);

    ssize_t read(void *buf, size_t count);
	ssize_t write(const void *buf, size_t count);

    ssize_t sendto(const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);
    ssize_t recvfrom(void *buffer, size_t length, int flags, struct sockaddr *address, socklen_t *address_len);

    ssize_t sendmsg(struct mmsghdr *msgvec, unsigned int vlen, int flags);
    ssize_t recvmsg(struct msghdr *message, int flags);
};

#endif /* __BYPASS_SOCK_H__ */