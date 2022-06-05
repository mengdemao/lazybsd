/**
 * @file init.cc
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

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[])
{
	bypass g_bypass;
    int fd = -1;

    if ((fd = g_bypass.open("test.cap", O_CREAT)) == -1)
    {
        std::cout << "Open file error" << std::endl;
        return EXIT_FAILURE;
    }

    g_bypass.close(fd);

    return EXIT_SUCCESS;
}
