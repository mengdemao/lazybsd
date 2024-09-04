/**
 * @file lazybsd_init.hh
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-05
 *
 * @brief
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#ifndef __LAZYBSD_INIT_HXX__
#define __LAZYBSD_INIT_HXX__

namespace lazybsd {

/**
 * @brief lazybsd init
 * @param  argc             init argc
 * @param  argv             init argv
 * @return int				EXIT_SUCESS/EXIT_FAILURE
 */
int init(int argc, char* argv[]);

/**
 * @brief  lazybsd exit
 * @param  status
 */
void exit(int status);

}

#endif /* __LAZYBSD_INIT_HXX__ */