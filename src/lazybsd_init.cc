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
#include <iostream>
#include <cstdlib>
#include <fmt/base.h>
#include <lazybsd.h>
#include "lazybsd_init.hh"
#include "lazybsd_cmdline.hh"
#include "lazybsd_log.h"

#include <ostream>
#include <rte_memory.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_debug.h>

namespace lazybsd {

/**
 * @brief lazybsd init funtion
 * @param  argc             init argc
 * @param  argv             init argv
 * @return int				LAZYBSD_EXIT_SUCCESS/LAZYBSD_EXIT_FAILURE
 */
int init(int argc, char* argv[])
{
	// parse cmdline value
	auto cmdline = lazybsd::cmdline::parse(argc, argv);
	if (cmdline.has_value()) {
		fmt::print("debug:{} config_file:{} proc_id:{} proc_type:{}\r\n",
			cmdline.value().debug, cmdline.value().config_file, 
			cmdline.value().proc_id, cmdline.value().proc_type);
	} else {
		return EXIT_FAILURE;
	}
	
	// init log 

	// load config file

	// init luajit plugin
	
	// init dpdk
	// auto ret = rte_eal_init(argc, argv);
	// if (ret < 0) {
	// 	rte_panic("Cannot init EAL\n");
	// }
	
	// freebsd system init

	// start veth interface 

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

}