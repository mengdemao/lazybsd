/**
 * @file test_memory.cc
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-05
 *
 * @brief 测试内存管理函数实现
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#include <gtest/gtest.h>
#include <lazybsd.h>

TEST(TEST_MEMORY, TEST_MALLOC)
{
	auto a = new int[1024];
	EXPECT_NE(a, nullptr);
	delete[] a;
}
