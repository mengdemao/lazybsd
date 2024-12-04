/**
 * @file lazybsd_global_ptr()->cc
 * @author Meng Demao (mengdemao19951021@163.com)
 * @brief lazybsd_global管理
 * @version 0.1
 * @date 2024-12-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <cstdint>
#include <cstring>
#include <helper.h>
#include <lazybsd.h>
#include "lazybsd_global.h"

#ifdef __cplusplus
extern "C" {
#endif

static lazybsd_global lazybsd_global_data;

/**
 * @brief get lazybsd_global_data ptr
 * 
 * @return lazybsd_global* 
 */
lazybsd_global* lazybsd_global_ptr(void)
{
    return &lazybsd_global_data;
}

/**
 * @brief print lazybsd global value
 * 
 */
void lazybsd_global_print(void)
{

}

/**
 * @brief set lazybsd_global_data default value
 */
CONSTRUCTOR_FUNCTION
void lazybsd_global_default(void)
{
    memset(&lazybsd_global_data, 0, sizeof(lazybsd_global));
}

#ifdef __cplusplus
}
#endif
