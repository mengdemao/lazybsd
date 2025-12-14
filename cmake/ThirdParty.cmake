# 添加所有的依赖
include(FetchContent)

# 使用FetchContent获取fmt库
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.2.0)

# 使用FetchContent获取gtest库
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0)

FetchContent_Declare(
    backward
    GIT_REPOSITORY https://github.com/bombela/backward-cpp
    GIT_TAG master)

# 确保依赖项被下载和构建
FetchContent_MakeAvailable(fmt googletest backward)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED CONFIG COMPONENTS program_options log filesystem regex date_time)

message(STATUS "Using backward library via FetchContent")
link_libraries(Backward::Backward)

# 链接fmt库
message(STATUS "Using fmt library via FetchContent")
include_directories(${fmt_SOURCE_DIR}/include)
link_libraries(fmt::fmt)

# 链接gtest库
message(STATUS "Using gtest library via FetchContent")
include_directories(${googletest_SOURCE_DIR}/googletest/include)
include_directories(${googletest_SOURCE_DIR}/googlemock/include)
link_libraries(gtest gtest_main gmock gmock_main)

if(OpenSSL_FOUND)
    message(STATUS "OpenSSL version is      : ${OPENSSL_VERSION}")
    message(STATUS "OpenSSL include path is : ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OpenSSL libraries is    : ${OPENSSL_LIBRARIES}")
    include_directories(${OPENSSL_INCLUDE_DIR})
    link_libraries(ssl crypto)
    add_definitions("-DOPENSSL_VERSION=\"${OPENSSL_VERSION}\"")
endif(OpenSSL_FOUND)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

if(Boost_FOUND)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    message(STATUS "Boost version is        : ${Boost_VERSION_STRING}")
    message(STATUS "Boost include path is   : ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries is      : ${Boost_LIBRARIES}")
    include_directories(${Boost_INCLUDE_DIRS})
    link_libraries(${Boost_LIBRARIES})
    add_definitions("-DBOOST_VERSION=\"${Boost_VERSION_STRING}\"")
endif(Boost_FOUND)

set(ENV{PKG_CONFIG_PATH}
    "${CMAKE_SOURCE_DIR}/install/lib/pkgconfig:${CMAKE_SOURCE_DIR}/install/lib/x86_64-linux-gnu/pkgconfig:$ENV{PKG_CONFIG_PATH}"
)
find_package(PkgConfig REQUIRED)
pkg_check_modules(DPDK REQUIRED libdpdk)
if(DPDK_FOUND)
    message(STATUS "DPDK version is         : ${DPDK_VERSION}")
    message(STATUS "DPDK include            : ${DPDK_INCLUDE_DIRS}")
    message(STATUS "DPDK libraries is       : ${DPDK_LIBRARIES}")
    message(STATUS "DPDK cflags is          : ${DPDK_CFLAGS}")
    message(STATUS "DPDK ldflags is         : ${DPDK_LDFLAGS}")
    link_directories(${CMAKE_SOURCE_DIR}/install/lib/x86_64-linux-gnu)
    link_directories(${CMAKE_SOURCE_DIR}/install/lib)
    list(APPEND DPDK_LIBRARIES "rte_net_bond")
    link_libraries(${DPDK_LIBRARIES})
    add_compile_options(${DPDK_CFLAGS})
    add_link_options(${DPDK_LDFLAGS})
    add_definitions("-DDPDK_VERSION=\"${DPDK_VERSION}\"")
endif(DPDK_FOUND)
