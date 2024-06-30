/**
 * @file lazybsd_bsd.h
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-06-22
 *
 * @brief
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#ifndef __LAZYBSD_BSD_H__
#define __LAZYBSD_BSD_H__

#ifdef __cplusplus
extern "C" {
#endif

extern int sendit(struct thread *td, int s, struct msghdr *mp, int flags);

extern void lazybsd_hardclock(void);

#ifdef __cplusplus
}
#endif

#endif /* __LAZYBSD_BSD_H__ */