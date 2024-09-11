#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <iostream>

#include <signal.h>
#include <execinfo.h>
#include <unistd.h>

#include <lazybsd_init.hh>
#include <lazybsd_socket.hh>

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

int backtrace_init(void)
{
	struct sigaction sa;

    sa.sa_sigaction = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;

    // 捕获SIGSEGV信号
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	int status = EXIT_SUCCESS;
	std::cout << "Hello Lazybsd Test Runner Start" << std::endl;

	backtrace_init();

	lazybsd::init(argc, argv);

	// int lazybsd_fd = lazybsd_socket(AF_INET, SOCK_STREAM, 0);
	// assert(lazybsd_fd >= 0);

	// status = lazybsd_close(lazybsd_fd);
	// assert(status == 0);

	lazybsd::exit(0);

	return status;
}
