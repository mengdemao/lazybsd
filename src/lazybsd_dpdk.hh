/**
 * @file lazybsd_dpdk.hh
 * @author mengdemao (mengdemao19951021@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-04-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef LAZYBSD_DPDK_HH
#define LAZYBSD_DPDK_HH
#include "lazybsd_dpdk.h"
#include <string>
#include <string_view>

namespace lazybsd::dpdk {

/**
 * @brief dpdk init
 *
 * @return int EXIT_SUCCESS/EXIT_FAILURE
 */
int init(void);

};  // namespace lazybsd::dpdk

#endif  // LAZYBSD_DPDK_HH