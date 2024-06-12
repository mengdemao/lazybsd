/**
 * @file lazybsd_kevent.h
 * @author Meng Demao (mengdemao19951021@163.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <sys/types.h>
#include <sys/event.h>
#ifndef __LAZYBSD_KEVENT_H__
#define __LAZYBSD_KEVENT_H__

int lazybsd_kqueue(void);

int lazybsd_kevent(int kq, const struct kevent *changelist, int nchanges,
    struct kevent *eventlist, int nevents, const struct timespec *timeout);
    
int lazybsd_kevent_do_each(int kq, const struct kevent *changelist, int nchanges,
    void *eventlist, int nevents, const struct timespec *timeout,
    void (*do_each)(void **, struct kevent *));

#endif /* __LAZYBSD_KEVENT_H__ */