/**
 * @file test_demo.cc
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-24
 *
 * @brief 测试程序模板
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */

#include <cstdint>
#include <cstdlib>
#include <lazybsd.h>
#include <lazybsd_args.h>
#include "test_main.hh"

TEST(TEST_ARGS, TEST_HELP)
{
	int   argc     = 2;
	char* argv[12] = {"test", "-h"};

	auto result = lazybsd::lazybsd_args(argc, argv);

	EXPECT_NE(result.has_value(), true);
}

TEST(TEST_ARGS, TEST_VERSION)
{
	int   argc     = 2;
	char* argv[12] = {"test", "-v"};

	auto result = lazybsd::lazybsd_args(argc, argv);

	EXPECT_NE(result.has_value(), true);
}

TEST(TEST_ARGS, TEST_DEBUG)
{
	int   argc     = 2;
	char* argv[12] = {"test", "-d"};

	auto result = lazybsd::lazybsd_args(argc, argv);

	EXPECT_EQ(result.has_value(), true);
}

TEST(TEST_ARGS, TEST_CONFIG)
{
	int   argc     = 3;
	char* argv[12] = {"test", "-c", "config.xml"};

	auto result = lazybsd::lazybsd_args(argc, argv);

	EXPECT_EQ(result.has_value(), true);
}

TEST(TEST_ARGS, TEST_PROC_ID)
{
	int   argc     = 3;
	char* argv[12] = {"test", "-p", "1"};

	auto result = lazybsd::lazybsd_args(argc, argv);

	EXPECT_EQ(result.has_value(), true);
}

TEST(TEST_ARGS, TEST_PROC_TYPE)
{
	int   argc     = 3;
	char* argv[12] = {"test", "-t", "master"};

	auto result = lazybsd::lazybsd_args(argc, argv);

	EXPECT_EQ(result.has_value(), true);
}
