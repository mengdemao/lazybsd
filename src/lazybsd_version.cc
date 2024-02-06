/**
 * @file lazybsd_version.cc
 * @author meng demao (mengdemao19951021@gmail.com)
 * @brief 版本信息获取
 * @version 0.1
 * @date 2022-09-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "lazybsd_version.h"

namespace lazybsd {
namespace version {

#include <version.h>

/**
 * @brief lazybsd 版本信息获取
 * @return struct lazybsd_version
 */
struct lazybsd_version lazybsd_version_get(void)
{
    struct lazybsd_version version;
    version.version_major = lazybsd_VERSION_MAJOR;
    version.version_minor = lazybsd_VERSION_MINOR;
    version.version_build = lazybsd_BUILD;
    return version;
}

/**
 * @brief 版本信息打印
 */
void lazybsd_version_print(void)
{
    fmt::print("lazybsd version: {} {}\r\n", lazybsd_VERSION, lazybsd_BUILD);
}

/**
 * @brief 版本字符串获取
 * @return std::string
 */
std::string lazybsd_version_string(void)
{
    return fmt::format("lazybsd version: {} {}", lazybsd_VERSION, lazybsd_BUILD);
}

}
}