/**
 * @file lazybsd_api.hh
 * @author mengdemao (mengdemao19951021@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "lazybsd_api.h"

#ifndef __LAZYBSD_API_HXX__
#define __LAZYBSD_API_HXX__

int lazybsd_init(int argc, char * const argv[]);

void lazybsd_run(loop_func_t loop, void *arg);

/* Tests if fd is used by F-Stack */
extern int lazybsd_fdisused(int fd);

extern int lazybsd_getmaxfd(void);

/*
 * Get traffic for QoS or other via API.
 * The size of buffer must >= siezof(struct lazybsd_traffic_args), now is 32 bytes.
 */
void lazybsd_get_traffic(void *buffer);

/*
 * On success, 0 is returned.
 * On error, -1 is returned, and errno is set appropriately.
 */
int lazybsd_route_ctl(enum LAZYBSD_ROUTE_CTL req, enum LAZYBSD_ROUTE_FLAG flag,
    struct linux_sockaddr *dst, struct linux_sockaddr *gw,
    struct linux_sockaddr *netmask);

/* regist a packet dispath function */
void lazybsd_regist_packet_dispatcher(dispatch_func_t func);

#endif // __LAZYBSD_API_HXX__
