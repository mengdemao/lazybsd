include(ExternalProject)
include(ProcessorCount)
ProcessorCount(N)

ExternalProject_Add(
    DPDK
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/dpdk
    INSTALL_DIR ${CMAKE_BINARY_DIR}/dpdk
    CONFIGURE_COMMAND meson setup --wipe --prefix=${CMAKE_BINARY_DIR}/dpdk -Dbuildtype=debug -Denable_kmods=true -Dexamples=all -Dplatform=native build
    BUILD_COMMAND ninja -C ${CMAKE_SOURCE_DIR}/dpdk/build -j${N}
    INSTALL_COMMAND ninja -C ${CMAKE_SOURCE_DIR}/dpdk/build install
    BUILD_IN_SOURCE TRUE
    USES_TERMINAL_CONFIGURE TRUE
    USES_TERMINAL_BUILD TRUE
    USES_TERMINAL_INSTALL TRUE
    EXCLUDE_FROM_ALL TRUE
)
