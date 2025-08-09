# 添加所有的依赖
find_package(GTest REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(
    Boost
    REQUIRED
    program_options
    log
    system
    filesystem
    regex
    date_time)

if(GTest_FOUND)
    message(STATUS "GTest version is      : ${GTest_VERSION_STRING}")
    message(STATUS "GTest include path is : ${GTest_INCLUDE_DIR}")
    message(STATUS "GTest libraries is    : ${GTest_LIBRARIES}")
    include_directories(${GTest_INCLUDE_DIR})
    link_libraries(${GTest_LIBRARIES})
endif(GTest_FOUND)

if(fmt_FOUND)
    message(STATUS "fmt version is      : ${fmt_VERSION_STRING}")
    message(STATUS "fmt include path is : ${fmt_INCLUDE_DIR}")
    message(STATUS "fmt libraries is    : ${fmt_LIBRARIES}")
    include_directories(${fmt_INCLUDE_DIR})
    link_libraries(${fmt_LIBRARIES})
endif(fmt_FOUND)

if(OpenSSL_FOUND)
    message(STATUS "OpenSSL version is      : ${OPENSSL_VERSION}")
    message(STATUS "OpenSSL include path is : ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OpenSSL libraries is    : ${OPENSSL_LIBRARIES}")
    include_directories(${OPENSSL_INCLUDE_DIR})
    link_libraries(${OPENSSL_LIBRARIES})
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

find_package(PkgConfig REQUIRED)
pkg_check_modules(DPDK REQUIRED libdpdk)
if(DPDK_FOUND)
    message(STATUS "DPDK version is        : ${DPDK_VERSION}")
    message(STATUS "DPDK libraries is      : ${DPDK_LIBRARIES}")
    message(STATUS "DPDK cflags is         : ${DPDK_CFLAGS}")
    message(STATUS "DPDK ldflags is         : ${DPDK_LDFLAGS}")
    link_libraries(${DPDK_LIBRARIES})
    add_compile_options(${DPDK_CFLAGS})
    add_link_options(${DPDK_LDFLAGS})
    add_definitions("-DDPDK_VERSION=\"${DPDK_VERSION}\"")
endif(DPDK_FOUND)
