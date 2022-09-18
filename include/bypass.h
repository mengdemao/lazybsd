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
#include <mutex>

using namespace std;

class bypass
{
private:
    string device;      // device name
    std::mutex mutex;   // device mutex 
public:
    bypass(string name)
    {
        device = name;
    }

    ~bypass()
    {
        /*  No Body */
    }

    virtual string devname(void)
    {
        return device;
    }

    virtual ssize_t read(void *buf, size_t count) = 0;
	virtual ssize_t write(const void *buf, size_t count) = 0;
};


#endif /* __BYPASS_H__ */
