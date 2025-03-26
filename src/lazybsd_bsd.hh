/**
 * @file lazybsd_bsd.hh
 * @author Meng Demao (mengdemao19951021@163.com)
 * @brief
 * @version 0.1
 * @date 2025-03-26
 *
 *
 */
#ifndef __LAZYBSD_BSD_HH__
#define __LAZYBSD_BSD_HH__
#include <cstdbool>
namespace lazybsd::bsd {

/**
 * @brief lazybsd freebsd proto stack init
 *
 * @return int EXIT_SUCCESS/EXIT_FAILURE
 */
static inline int init(void)
{
    return EXIT_SUCCESS;
}

};  // namespace lazybsd::bsd

#endif /* __LAZYBSD_BSD_HH__ */