find_package(Git REQUIRED)

option(USE_BYPASS_SOCK "use sock to bypass ethernet" ON)
option(USE_BYPASS_FILE "use file to bypass ethernet" OFF)
option(USE_BYPASS_DPDK "use dpdk to bypass ethernet" OFF)

# 设置版本号
if (GIT_FOUND)
  # 主版本号
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0 HEAD
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
  # 次版本号
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    OUTPUT_VARIABLE GIT_SHA
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif (GIT_FOUND)

set(${PROJECT_NAME}_VERSION_MAJOR ${GIT_TAG})
set(${PROJECT_NAME}_VERSION_MINOR ${GIT_SHA})

# 提取编译信息
string(TIMESTAMP COMPILE_TIME %Y/%m/%d-%H:%M:%S)
set(BUILD_TIME ${COMPILE_TIME})
cmake_host_system_information(RESULT BUILD_HOST QUERY HOSTNAME)

# 生成配置文件
configure_file(${CMAKE_SOURCE_DIR}/inc/config.h.in	${CMAKE_SOURCE_DIR}/inc/generated/config.h)