/**
 * @file lazybsd_socket.h
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-05
 *
 * @brief lazybsd socket
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#include <lazybsd.h>
#ifndef __LAZYBSD_SOCKET_H__
#define __LAZYBSD_SOCKET_H__

#ifdef __cplusplus
extern "C" {
#endif

#define AF_INET6_LINUX      10
#define PF_INET6_LINUX      AF_INET6_LINUX
#define AF_INET6_FREEBSD    28
#define PF_INET6_FREEBSD    AF_INET6_FREEBSD

#ifdef __cplusplus
}
#endif

#endif // __LAZYBSD_SOCKET_H__
