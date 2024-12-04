/**
 * @file lazybsd_luajit.cc
 * @author Meng Demao (mengdemao19951021@163.com)
 * @brief luajit插件
 * @version 0.1
 * @date 2024-12-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <lazybsd_luajit.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define luajit_c

#include <lua.hpp>
#include <unistd.h>