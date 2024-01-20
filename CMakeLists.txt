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
add_compile_options(-Wall -Wno-unused -gdwarf-5 -fno-common -fstack-protector)

# 添加外部软件
find_package(Threads REQUIRED)
find_package(Doxygen REQUIRED)

# 添加所有的conan包
find_package(GTest REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED program_options system filesystem regex date_time)

if(GTest_FOUND)
  message(STATUS "GTest include path is : ${GTest_INCLUDE_DIR}")
  message(STATUS "GTest libraries is    : ${GTest_LIBRARIES}")
  include_directories(${GTest_INCLUDE_DIR})
  link_libraries(${GTest_LIBRARIES})
endif(GTest_FOUND)

if(fmt_FOUND)
  message(STATUS "fmt include path is : ${fmt_INCLUDE_DIR}")
  message(STATUS "fmt libraries is    : ${fmt_LIBRARIES}")
  include_directories(${fmt_INCLUDE_DIR})
  link_libraries(${fmt_LIBRARIES})
endif(fmt_FOUND)

if(OpenSSL_FOUND)
  message(STATUS "OpenSSL include path is : ${OPENSSL_INCLUDE_DIR}")
  message(STATUS "OpenSSL libraries is    : ${OPENSSL_LIBRARIES}")
  include_directories(${OPENSSL_INCLUDE_DIR})
  link_libraries(${OPENSSL_LIBRARIES})
endif(OpenSSL_FOUND)

if(Boost_FOUND)
  set(Boost_USE_STATIC_LIBS        ON)
  set(Boost_USE_MULTITHREADED      ON)
  set(Boost_USE_STATIC_RUNTIME    OFF)
  message(STATUS "Boost include path is   : ${Boost_INCLUDE_DIRS}")
  message(STATUS "Boost libraries is      : ${Boost_LIBRARIES}")
  include_directories(${Boost_INCLUDE_DIRS})
  link_libraries(${Boost_LIBRARIES})
endif(Boost_FOUND)

# 添加头文件路径
include_directories(${CMAKE_SOURCE_DIR}/include/generated)
include_directories(${CMAKE_SOURCE_DIR}/include)

# 添加子文件夹
add_subdirectory(${CMAKE_SOURCE_DIR}/bsd ${CMAKE_BINARY_DIR}/bsd)
add_subdirectory(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib ${CMAKE_BINARY_DIR}/lib)
add_subdirectory(${CMAKE_SOURCE_DIR}/test ${CMAKE_BINARY_DIR}/test)
