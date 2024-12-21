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

#include <cstdlib>
#include <lazybsd.h>
#include <lazybsd_log.h>
#include "test_main.hh"

TEST(TEST_LOG, TEST_LOG)
{
    log_trace << "A trace severity message";
    log_debug << "A debug severity message";
    log_info << "An informational severity message";
    log_warning << "A warning severity message";
    log_error << "An error severity message";
    log_fatal << "A fatal severity message";
	EXPECT_NE(EXIT_FAILURE, EXIT_SUCCESS);
}
