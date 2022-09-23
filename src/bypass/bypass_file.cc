/**
 * @file file.cc
 * @author mengdemao (you@domain.com)
 * @brief 网卡实现(file)
 * @version 0.1
 * @date 2022-05-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <iostream>
#include <config.h>
#include <bypass.h>
#include "bypass_file.h"

#ifdef USE_BYPASS_FILE

ssize_t bypass_file::read(void *buf, size_t count)
{
    return 0;
}

ssize_t bypass_file::write(const void *buf, size_t count)
{
    return 0;
}

#endif /* USE_BYPASS_FILE */
