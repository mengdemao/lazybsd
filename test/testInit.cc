/**
 * @file testInit.cc
 * @author mengdemao (mengdemao19951021@163.com)
 * @brief 初始化测试
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

TEST(testInit, testInit)
{
    struct lazybsd_version lazybsd_version;
    lazybsd_version_get(lazybsd_version);
    fmt::print("version_major {} \nversion_minor {} \nversion_build {} \n", 
                lazybsd_version.version_major, 
                lazybsd_version.version_minor, 
                lazybsd_version.version_build);
}