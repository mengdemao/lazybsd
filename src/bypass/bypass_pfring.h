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

class bypass_pfring: public bypass {
private:
    int fd;
    int domain;
    int type;
    int protocol;

public:
    int open(const char *pathname, int flags);
    int close(void);

    ssize_t read(void *buf, size_t count);
	ssize_t write(const void *buf, size_t count);
};

#endif /* __FILE_H__ */