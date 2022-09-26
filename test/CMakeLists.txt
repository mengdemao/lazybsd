cmake_minimum_required(VERSION 3.24)

project(test C CXX ASM)

find_package(GTest)
enable_testing()

add_executable(testInit testInit.cc)
target_link_libraries(testInit gtest gtest_main Threads::Threads bsd bypass wrapper lazybsd fmt)
gtest_add_tests(TARGET      testInit
                TEST_SUFFIX .noArgs
                TEST_LIST   testInit
)
set_tests_properties(${testInit} PROPERTIES TIMEOUT 10)

add_executable(testBypassSock testBypassSock.cc)
target_link_libraries(testBypassSock bypass gtest gtest_main Threads::Threads)
gtest_add_tests(TARGET      testBypassSock
                TEST_SUFFIX .noArgs
                TEST_LIST   testBypassSock
)
set_tests_properties(${testBypassSock} PROPERTIES TIMEOUT 10)