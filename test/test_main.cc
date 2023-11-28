/**
 * @file test_main.cc
 * @author mengdemao (mengdemao19951021@163.com)
 * @brief 测试功能起点
 * @version 0.1
 * @date 2023-11-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <iostream>
#include <gtest/gtest.h>
#include <lazybsd.h>
#include <fmt/core.h>
#include <helper.h>

/**
 * @brief  函数起点
 * @param  main 参数个数
 * @param  argv 参数列表
 * @return int  运行结果
 */
int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);

	struct lazybsd_version lazybsd_version = lazybsd_version_get();
    fmt::print("lazybsd version {} {} {} \n",
                lazybsd_version.version_major,
                lazybsd_version.version_minor,
                lazybsd_version.version_build);

	return RUN_ALL_TESTS();
}