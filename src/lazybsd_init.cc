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
#include <lazybsd.h>
#include "lazybsd_init.hh"

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
	auto ret = rte_eal_init(argc, argv);
	if (ret < 0) {
		rte_panic("Cannot init EAL\n");
	}

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