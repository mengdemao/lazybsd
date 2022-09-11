/**
 * @file pcap.h
 * @author mengdemao (mengdemao19951021@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __FILE_H__
#define __FILE_H__

#include <bypass.h>

class bypass_dpdk: public bypass {
    int open(const char *pathname, int flags);
    int close(int fd);

    ssize_t read(int fd, void *buf, size_t count);
	ssize_t write(int fd, const void *buf, size_t count);
};

#endif /* __FILE_H__ */