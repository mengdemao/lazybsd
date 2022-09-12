/**
 * @file file.cc
 * @author mengdemao (you@domain.com)
 * @brief 网卡实现(file)
 * @version 0.1
 * @date 2022-05-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <iostream>
#include <config.h>
#include <bypass.h>
#include <cstdlib>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>

#include "bypass_sock.h"

#ifdef USE_BYPASS_SOCK

bypass_sock::bypass_sock(string device, int domain, int type, int protocol)
: bypass(device)
{
    this->domain = domain;
    this->type = type; 
    this->protocol = protocol;

    if (0 > (fd = socket(this->domain, this->type, this->protocol))) {
        std::cout << "socket create error " <<  strerror(errno) << std::endl;
    }
}

bypass_sock::~bypass_sock()
{
    if (close(this->fd) != 0)
    {
        std::cout << "socket close error " <<  strerror(errno) << std::endl;
    }
}

int bypass_sock::bind(struct sockaddr *addr, socklen_t addrlen)
{
    return ::bind(this->fd, addr, addrlen);
}

int bypass_sock::listen(int backlog)
{
    return ::listen(this->fd, backlog);
}

int bypass_sock::connect(const struct sockaddr *addr, socklen_t addrlen)
{
    return ::connect(this->fd, addr, addrlen);
}

int bypass_sock::accept(struct sockaddr *addr, socklen_t *addrlen)
{   
    return ::accept(this->fd, addr, addrlen);
}

ssize_t bypass_sock::read(void *buf, size_t count)
{
    return ::read(this->fd, buf, count);
}

ssize_t bypass_sock::write(const void *buf, size_t count)
{
    return ::write(this->fd, buf, count);
}

ssize_t bypass_sock::sendto(const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len)
{
    return ::sendto(this->fd, message, length, flags, dest_addr, dest_len);
}

ssize_t bypass_sock::recvfrom(void *buffer, size_t length, int flags, struct sockaddr *address, socklen_t *address_len)
{
    return ::recvfrom(this->fd, buffer, length, flags, address, address_len);
}

ssize_t bypass_sock::sendmsg(struct mmsghdr *msgvec, unsigned int vlen, int flags)
{
    return ::sendmmsg(this->fd, msgvec, vlen, flags);
}

ssize_t bypass_sock::recvmsg(struct msghdr *message, int flags)
{
    return ::recvmsg(this->fd, message, flags);
}


#endif /* USE_BYPASS_SOCK */
