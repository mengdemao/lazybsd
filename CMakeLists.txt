cmake_minimum_required(VERSION 3.24)

project(fakenet C CXX ASM)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_SOURCE_DIR}/cmake)

include(CheckCompiler)
include(MakeConfig)
include(ThirdParty)

# 设置多线程编译
include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
  set(CTEST_BUILD_FLAGS -j${N})
  set(ctest_test_args ${ctest_test_args} PARALLEL_LEVEL ${N})
  set(CMAKE_MAKE_PROGRAM"${CMAKE_MAKE_PROGRAM} -j ${N}")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 显示详细信息
set (CMAKE_VERBOSE_MAKEFILE ON)

# 设置默认编译类型
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# 添加外部软件
find_package(Boost REQUIRED system filesystem regex date_time)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(Doxygen REQUIRED)

# 添加头文件路径
include_directories(${CMAKE_SOURCE_DIR}/inc/generated)
include_directories(${CMAKE_SOURCE_DIR}/inc)

# 添加子文件夹 
add_subdirectory(${CMAKE_SOURCE_DIR}/bsd ${CMAKE_BINARY_DIR}/bsd)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib ${CMAKE_BINARY_DIR}/lib)
add_subdirectory(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/test ${CMAKE_BINARY_DIR}/test)