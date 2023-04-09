/**
 * @file test_version.cc
 * @author mengdemao (mengdemao19951021@163.com)
 * @brief 打印版本号
 * @version 0.1
 * @date 2022-05-29
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <iostream>
#include <bypass.h>
#include <gtest/gtest.h>
#include <lazybsd.h>
#include <fmt/core.h>
#include <helper.h>

TEST_HEAD
{
    struct lazybsd_version lazybsd_version = lazybsd_version_get();
    fmt::print("version_major {} \nversion_minor {} \nversion_build {} \n",
                lazybsd_version.version_major,
                lazybsd_version.version_minor,
                lazybsd_version.version_build);
}