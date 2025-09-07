cmake_minimum_required(VERSION 3.24)
project(lazybsd C CXX ASM)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include(CheckCompiler)
include(MakeConfig)
include(BuildDPDK)
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
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 显示详细信息
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

# 设置默认编译类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# 添加公共的编译指令
add_compile_options(
    -Wall
    -O0
    -g
    -Wno-unused
    -gdwarf-4
    -fno-common
    -fstack-protector
    -fPIC
    -Wno-stringop-overread)

add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wno-missing-prototypes>)
add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wno-stringop-overread>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-result>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-pointer-arith>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-format>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-sign-compare>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-sign-compare>)

# 设置include头文件
add_compile_options(-include${CMAKE_SOURCE_DIR}/include/lazybsd.h)

# 强制编译器输出颜色
add_compile_options(-fdiagnostics-color=auto)

# 设置CPU Flags
add_compile_options(-march=native)

# 设置coverage编译指令
option(BUILD_COVERAGE "build program in coverage mode" OFF)

# 设置LTO优化指令
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

# 添加外部软件
find_package(Threads REQUIRED)
find_package(Doxygen REQUIRED)

# 添加源文件
aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC_LAZYBSD)

# 生成目标
add_library(${PROJECT_NAME} STATIC ${SRC_LAZYBSD})
target_compile_definitions(${PROJECT_NAME} PUBLIC LAZYBSD_USE_PAGE_ARRAY)

# 添加可执行文件
add_executable(${PROJECT_NAME}_runner main.cc)
target_link_libraries(${PROJECT_NAME}_runner PRIVATE "$<LINK_GROUP:RESCAN,${PROJECT_NAME},freebsd>")
target_link_options(${PROJECT_NAME}_runner PRIVATE -Xlinker -Map=${PROJECT_NAME}_runner.map)

# 添加头文件路径
include_directories(${CMAKE_SOURCE_DIR}/include/generated)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/bsd/lib)

# 添加子文件夹
add_subdirectory(${CMAKE_SOURCE_DIR}/bsd ${CMAKE_BINARY_DIR}/bsd)
add_subdirectory(${CMAKE_SOURCE_DIR}/test ${CMAKE_BINARY_DIR}/test)

# 安装配置
install(
    TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_runner
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

# 安装头文件
install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "generated" EXCLUDE)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/generated/
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.h")

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/bsd/include/
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.h")
