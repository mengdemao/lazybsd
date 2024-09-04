#include <cassert>
#include <cstdlib>
#include <iostream>

#include <lazybsd_init.hh>
#include <lazybsd_socket.hh>

int main(int argc, char *argv[])
{
	int status = EXIT_SUCCESS;
	std::cout << "Hello Lazybsd Test Runner Start" << std::endl;

	lazybsd::init(argc, argv);

	int lazybsd_fd = lazybsd_socket(AF_INET, SOCK_STREAM, 0);
	assert(lazybsd_fd >= 0);

	status = lazybsd_close(lazybsd_fd);
	assert(status == 0);

	lazybsd::exit(0);

	return status;
}