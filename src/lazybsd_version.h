/**
 * @file lazybsd_version.h
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-04
 *
 * @brief
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#include <lazybsd.h>
#ifndef __LAZYBSD_VERSION__
#define __LAZYBSD_VERSION__

namespace lazybsd {
namespace version {

struct lazybsd_version {
    std::string version_major;
    std::string version_minor;
    std::string version_build;
};

/**
 * @brief lazybsd 版本信息获取
 * @return struct lazybsd_version
 */
struct lazybsd_version lazybsd_version_get(void);

/**
 * @brief 版本信息打印
 */
void lazybsd_version_print(void);

/**
 * @brief 版本字符串获取
 * @return std::string
 */
std::string lazybsd_version_string(void);

}
}

#endif /* __LAZYBSD_VERSION__ */