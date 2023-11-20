cmake_minimum_required(VERSION 3.24)

project(test C CXX ASM)

enable_testing()

include(GoogleTest)
find_package(GTest)

add_executable(test_pfring test_main.cc test_pfring.cc)
add_executable(test_socket test_main.cc test_socket.cc)

target_link_libraries(test_pfring gtest Threads::Threads bsd bypass wrapper lazybsd fmt)
target_link_libraries(test_socket gtest Threads::Threads bsd bypass wrapper lazybsd fmt)

gtest_discover_tests(test_pfring)
gtest_discover_tests(test_socket)
