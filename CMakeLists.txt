cmake_minimum_required(VERSION 3.24)

project(lazybsd C CXX ASM)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_SOURCE_DIR}/cmake)

include(CheckCompiler)
include(MakeConfig)

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

# 添加公共的编译指令
add_compile_options(-Wall -Wno-unused -gdwarf-2 -g3 -fno-common -fstack-protector)

# 三方库
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

# 添加外部软件
find_package(Threads REQUIRED)
find_package(Doxygen REQUIRED)
find_package(GTest REQUIRED)
find_package(fmt REQUIRED)

# 添加头文件路径
include_directories(${CMAKE_SOURCE_DIR}/include/generated)
include_directories(${CMAKE_SOURCE_DIR}/include)

# 添加子文件夹
add_subdirectory(${CMAKE_SOURCE_DIR}/bsd ${CMAKE_BINARY_DIR}/bsd)
add_subdirectory(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/test ${CMAKE_BINARY_DIR}/test)
