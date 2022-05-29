/**
 * @file impl.h
 * @author Meng Demao (mengdemao19951021@163.com)
 * @brief 网卡实现接口
 * @version 0.1
 * @date 2022-05-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __IMPL_H__
#define __IMPL_H__
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

class impl
{
private:
    string s_name;
public:
    impl();
    ~impl();

    string name(void);

    void init(void);
    void exit(void);
    
    int open(const char *pathname, int flags);
    int close(int fd);

    ssize_t read(int fd, void *buf, size_t count);
	ssize_t write(int fd, const void *buf, size_t count);
};

/**
 * @brief Construct a new impl::impl object
 * 
 * @param name 
 */
impl::impl()
{
    init();
}

/**
 * @brief Destroy the impl::impl object
 * 
 */
impl::~impl()
{
    exit();
}

/**
 * @brief 输出模块名
 * 
 * @return string 
 */
string impl::name(void)    
{
    this->s_name;
}

#endif /* __IMPL_H__ */