/**
 * @file lazybsd_init.cc
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-05
 *
 * @brief
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#include "lazybsd_init.hh"
#include "lazybsd_cmdline.hh"
#include "lazybsd_log.h"
#include <cstdlib>
#include <fmt/base.h>
#include <lazybsd.h>
#include <lazybsd_bsd.hh>
#include <lazybsd_cfg.hh>
#include <lazybsd_dpdk.hh>
#include <lazybsd_luajit.h>
#include <lazybsd_osa.hh>
#include <lazybsd_veth.hh>

#include <rte_debug.h>
#include <rte_eal.h>
#include <rte_launch.h>
#include <rte_lcore.h>
#include <rte_memory.h>
#include <rte_per_lcore.h>

namespace lazybsd {

/**
 * @brief lazybsd init funtion
 * @param  argc             init argc
 * @param  argv             init argv
 * @return int				LAZYBSD_EXIT_SUCCESS/LAZYBSD_EXIT_FAILURE
 */
int init(int argc, char* argv[])
{
    lazybsd::log::init();

    lazybsd::cfg::init();

    lazybsd::osa::init();

    lazybsd::dpdk::init();

    lazybsd::bsd::init();

    lazybsd::veth::init();

    return LAZYBSD_EXIT_SUCCESS;
}

/**
 * @brief	lazybsd exit function
 * @param  status        exit status
 */
void exit(int status)
{
    rte_eal_cleanup();
}

}  // namespace lazybsd