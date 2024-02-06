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
#define JEMALLOC_NO_DEMANGLE
#include <gtest/gtest.h>
#include <jemalloc/jemalloc.h>
#include <lazybsd.h>

TEST(TEST_MEMORY, TEST_JEMALLOC)
{
	auto a = je_malloc(1024 * 10);
	EXPECT_NE(a, nullptr);
	je_free(a);
}

TEST(TEST_MEMORY, TEST_JEMALLOC_CXX)
{
	auto a = new int[1024];
	EXPECT_NE(a, nullptr);
	delete[] a;
}
