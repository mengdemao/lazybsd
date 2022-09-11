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
#include "bypass_dpdk.h"

#ifdef USE_BYPASS_FILE

int bypass_dpdk::open(const char *pathname, int flags)
{
    return EXIT_SUCCESS;
}

int bypass_dpdk::close(int fd)
{
    return EXIT_SUCCESS;
}

ssize_t bypass_dpdk::read(int fd, void *buf, size_t count)
{
    return 0;
}

ssize_t bypass_dpdk::write(int fd, const void *buf, size_t count)
{
    return 0;
}

#endif /* USE_BYPASS_FILE */
