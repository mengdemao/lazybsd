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

class bypass_sock:public bypass {
    int open(const char *pathname, int flags);
    int close(int fd);

    ssize_t read(int fd, void *buf, size_t count);
	ssize_t write(int fd, const void *buf, size_t count);
};

#endif /* __BYPASS_SOCK_H__ */