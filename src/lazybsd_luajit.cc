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
#include <fmt/base.h>
#include <lazybsd_luajit.h>
#include <lazybsd_global.hh>
#include <lazybsd.h>
#include <helper.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define luajit_c

#include <lua.hpp>
#include <unistd.h>

namespace lazybsd {

/**
 * @brief 
 * 
 */
CONSTRUCTOR_FUNCTION
void luajit_init(void)
{
    auto L = static_cast<lua_State *>(lazybsd_global_ptr()->L);
    L = lua_open();

    if (L == NULL) {
        fmt::print("lua_open error\r\n");
    }
}

/**
 * @brief lazybsd_luajit_exit
 * 
 */
DESTRUCTOR_FUNCTION
void luajit_exit(void)
{
    auto L = static_cast<lua_State *>(lazybsd_global_ptr()->L);
    lua_close(L);
}

};
