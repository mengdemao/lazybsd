/**
 * @file lazybsd_backtrace.c
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-12-04
 *
 * @brief lazybsd backtrace管理
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>
#include <lazybsd.h>
#include <helper.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief signal handler function
 * 
 * @param signum 
 * @param info 
 * @param ptr 
 */
static void signal_handler(int signum, siginfo_t *info, void *ptr)
{
    void *array[10];
    size_t size;
    char **strings;
    size_t i;

    // 获取当前线程的堆栈跟踪
    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);

    printf("Obtained %zd stack frames.\n", size);

    for (i = 0; i < size; i++) {
        printf("%s\n", strings[i]);
    }

    free(strings); // 释放backtrace_symbols分配的内存
    exit(signum);
}

CONSTRUCTOR_FUNCTION
void backtrace_init(void)
{
	struct sigaction sa;

    sa.sa_sigaction = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;

    // 捕获SIGSEGV信号
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction");
    }
}
