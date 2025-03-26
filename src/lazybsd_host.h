/**
 * @file lazybsd_host.h
 * @author mengdemao (mengdemao19951021@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-04-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef LAZYBSD_HOST_INTERFACE_H
#define LAZYBSD_HOST_INTERFACE_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define lazybsd_PROT_NONE   0x00
#define lazybsd_PROT_READ   0x01
#define lazybsd_PROT_WRITE  0x02

#define lazybsd_MAP_SHARED  0x0001
#define lazybsd_MAP_PRIVATE 0x0002
#define lazybsd_MAP_ANON    0x1000
#define lazybsd_MAP_NOCORE  0x00020000

#define lazybsd_MAP_FAILED  ((void*)-1)

void* lazybsd_mmap(void* addr, uint64_t len, int prot, int flags, int fd, uint64_t offset);
int   lazybsd_munmap(void* addr, uint64_t len);

void* lazybsd_malloc(uint64_t size);
void* lazybsd_calloc(uint64_t number, uint64_t size);
void* lazybsd_realloc(void* p, uint64_t size);
void  lazybsd_free(void* p);

#define lazybsd_CLOCK_REALTIME       0
#define lazybsd_CLOCK_MONOTONIC      4
#define lazybsd_CLOCK_MONOTONIC_FAST 12

#define lazybsd_NSEC_PER_SEC         (1000ULL * 1000ULL * 1000ULL)

void     lazybsd_clock_gettime(int id, int64_t* sec, long* nsec);
uint64_t lazybsd_clock_gettime_ns(int id);
uint64_t lazybsd_get_tsc_ns(void);

void lazybsd_get_current_time(int64_t* sec, long* nsec);
void lazybsd_update_current_ts(void);

typedef volatile uintptr_t lazybsd_mutex_t;
typedef void*              lazybsd_cond_t;
typedef void*              lazybsd_rwlock_t;

void     lazybsd_arc4rand(void* ptr, unsigned int len, int reseed);
uint32_t lazybsd_arc4random(void);

int   lazybsd_setenv(const char* name, const char* value);
char* lazybsd_getenv(const char* name);

void lazybsd_os_errno(int error);

int lazybsd_in_pcbladdr(uint16_t family, void* faddr, uint16_t fport, void* laddr);

int lazybsd_rss_check(void* softc, uint32_t saddr, uint32_t daddr, uint16_t sport, uint16_t dport);

#ifdef __cplusplus
}
#endif

#endif /* LAZYBSD_HOST_INTERFACE_H */
