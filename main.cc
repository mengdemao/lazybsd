#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <fmt/core.h>
#include <lazybsd_cmdline.hh>
#include <lazybsd_init.hh>
#include <lazybsd_socket.hh>

#define BACKWARD_HAS_DW 1
#include "backward.hpp"

namespace backward {
backward::SignalHandling sh;
}

/**
 * @brief lazybsd main entry
 *
 * @param argc argument count
 * @param argv argument array
 * @return int EXIT_SUCCESS/EXIT_FAILURE
 */
int main(int argc, char* argv[])
{
    int status = EXIT_SUCCESS;
    std::cout << "lazybsd runner start" << std::endl;

    auto cmdline = lazybsd::cmdline::parse(argc, argv);
    if (cmdline.has_value()) {
        fmt::print("debug:{} config_file:{} proc_id:{} proc_type:{}\r\n", cmdline.value().debug, cmdline.value().config_file, cmdline.value().proc_id, cmdline.value().proc_type);
    } else {
        return EXIT_FAILURE;
    }

    status = lazybsd::init(argc, argv);
    if (status) {
        return EXIT_FAILURE;
    }

    auto lazybsd_fd = lazybsd_socket(AF_INET, SOCK_STREAM, 0);
    assert(lazybsd_fd >= 0);

    status = lazybsd_close(lazybsd_fd);
    assert(status == 0);

    lazybsd::exit(0);

    return status;
}
