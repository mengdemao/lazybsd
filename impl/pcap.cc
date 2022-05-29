/**
 * @file pcap.cc
 * @author mengdemao (you@domain.com)
 * @brief 网卡实现(pcap)
 * @version 0.1
 * @date 2022-05-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <iostream>
#include <config.h>
#include <impl.h>

#ifdef CONFIG_IMPL_PCAP

/**
 * @brief 网络实现初始化
 * 
 */
void impl::init(void)
{
    this->s_name = "pcap";
}

/**
 * @brief 网络实现退出
 * 
 */
void impl::exit(void)
{
}

int impl::open(const char *pathname, int flags)
{
    return EXIT_SUCCESS;
}

int impl::close(int fd)
{
    return EXIT_SUCCESS;
}

ssize_t impl::read(int fd, void *buf, size_t count)
{
    return 0;
}

ssize_t impl::write(int fd, const void *buf, size_t count)
{
    return 0;
}

#endif /* CONFIG_IMPL_PCAP */