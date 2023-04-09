/**
 * @file lazybsd.h
 * @author mengdemao (mengdemao@google.com)
 * @brief freebsd协议栈接口文件
 * @version 0.1
 * @date 2022-09-11
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __LAZYBSD_H__
#define __LAZYBSD_H__

struct lazybsd_version {
    std::string version_major;
    std::string version_minor;
    std::string version_build;
};

struct lazybsd_version lazybsd_version_get(void);

#endif /* __LAZYBSD_H__ */