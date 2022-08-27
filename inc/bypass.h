/**
 * @file bypass.h
 * @author Meng Demao (mengdemao19951021@163.com)
 * @brief 网卡透传数据接口
 * @version 0.1
 * @date 2022-05-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __BYPASS_H__
#define __BYPASS_H__
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

class bypass
{
private:
    string s_name;
public:
    bypass();
    ~bypass();

    string name(void);
    
    int open(const char *pathname, int flags);
    int close(int fd);

    ssize_t read(int fd, void *buf, size_t count);
	ssize_t write(int fd, const void *buf, size_t count);
};

/**
 * @brief Construct a new bypass::bypass object
 * 
 * @param name 
 */
bypass::bypass()
{
}

/**
 * @brief Destroy the bypass::bypass object
 * 
 */
bypass::~bypass()
{
}

#endif /* __BYPASS_H__ */
