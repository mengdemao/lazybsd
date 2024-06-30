cmake_minimum_required(VERSION 3.24)
project(lazybsd C CXX ASM)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

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
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 显示详细信息
set(CMAKE_VERBOSE_MAKEFILE ON)

# 设置默认编译类型
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# 添加公共的编译指令
add_compile_options(-Wall -Wno-unused -gdwarf-5 -fno-common -fstack-protector -fPIC)

# 强制编译器输出颜色
add_compile_options(-fdiagnostics-color=auto)

# 设置CPU Flags
add_compile_options(-march=native)

# 设置coverage编译指令
option(BUILD_COVERAGE "build programe in coverage mode" OFF)

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

# 添加所有的conan包
find_package(GTest REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(
  Boost
  REQUIRED
  program_options
  system
  filesystem
  regex
  date_time)

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
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_MULTITHREADED ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
  message(STATUS "Boost include path is   : ${Boost_INCLUDE_DIRS}")
  message(STATUS "Boost libraries is      : ${Boost_LIBRARIES}")
  include_directories(${Boost_INCLUDE_DIRS})
  link_libraries(${Boost_LIBRARIES})
endif(Boost_FOUND)

# 添加DPDK库文件路径
include_directories(${CMAKE_SOURCE_DIR}/install/include)
link_directories(${CMAKE_SOURCE_DIR}/install/lib)
link_directories(${CMAKE_SOURCE_DIR}/install/lib/x86_64-linux-gnu)

set(DPDK_LIBRARIES
    rte_mbuf
    rte_crypto_nitrox
    rte_argparse
    rte_net_ena
    rte_net_enetfec
    rte_net_atlantic
    rte_net
    rte_net_hinic
    rte_bus_uacce
    rte_raw_cnxk_gpio
    rte_member
    rte_cryptodev
    rte_graph
    rte_event_dpaa2
    rte_net_i40e
    rte_common_octeontx
    rte_reorder
    rte_raw_ifpga
    rte_common_dpaax
    rte_common_cpt
    rte_ip_frag
    rte_pcapng
    rte_baseband_null
    rte_event_dsw
    rte_regex_cn9k
    rte_mempool_dpaa2
    rte_event_sw
    rte_net_af_packet
    rte_crypto_caam_jr
    rte_port
    rte_net_nfp
    rte_pdcp
    rte_regexdev
    rte_net_iavf
    rte_mempool_stack
    rte_net_ark
    rte_ring
    rte_baseband_fpga_5gnr_fec
    rte_pci
    rte_crypto_virtio
    rte_baseband_turbo_sw
    rte_compress_octeontx
    rte_net_pfe
    rte_ipsec
    rte_baseband_la12xx
    rte_event_dlb2
    rte_latencystats
    rte_net_cnxk
    rte_raw_ntb
    rte_mempool
    rte_rawdev
    rte_mempool_dpaa
    rte_net_sfc
    rte_vdpa_nfp
    rte_efd
    rte_common_qat
    rte_net_softnic
    rte_net_ngbe
    rte_net_vhost
    rte_bus_vmbus
    rte_net_failsafe
    rte_dma_dpaa2
    rte_power
    rte_compressdev
    rte_net_thunderx
    rte_sched
    rte_net_netvsc
    rte_baseband_fpga_lte_fec
    rte_net_enic
    rte_bus_cdx
    rte_net_gve
    rte_vdpa_sfc
    rte_net_cxgbe
    rte_net_memif
    rte_dma_hisilicon
    rte_bus_ifpga
    rte_net_virtio
    rte_dma_cnxk
    rte_net_bnx2x
    rte_net_avp
    rte_node
    rte_net_null
    rte_net_e1000
    rte_crypto_cnxk
    rte_bitratestats
    rte_dma_idxd
    rte_net_octeon_ep
    rte_net_octeontx
    rte_baseband_acc
    rte_dma_skeleton
    rte_net_igc
    rte_net_ice
    rte_net_idpf
    rte_event_octeontx
    rte_common_sfc_efx
    rte_mldev
    rte_bus_platform
    rte_event_skeleton
    rte_event_opdl
    rte_crypto_ccp
    rte_crypto_bcmfs
    rte_net_af_xdp
    rte_net_ionic
    rte_bus_pci
    rte_metrics
    rte_common_iavf
    rte_net_pcap
    rte_mempool_cnxk
    rte_fib
    rte_rcu
    rte_eventdev
    rte_vdpa_ifc
    rte_mempool_octeontx
    rte_stack
    rte_net_ring
    rte_net_ipn3ke
    rte_mempool_bucket
    rte_bus_auxiliary
    rte_net_axgbe
    rte_bpf
    rte_net_bnxt
    rte_net_dpaa2
    rte_rib
    rte_cmdline
    rte_log
    rte_telemetry
    rte_lpm
    rte_crypto_octeontx
    rte_ml_cnxk
    rte_net_bond
    rte_net_vdev_netvsc
    rte_bbdev
    rte_bus_vdev
    rte_crypto_scheduler
    rte_net_dpaa
    rte_crypto_openssl
    rte_net_enetc
    rte_net_cpfl
    rte_timer
    rte_pipeline
    rte_cfgfile
    rte_gpudev
    rte_gro
    rte_raw_skeleton
    rte_raw_cnxk_bphy
    rte_compress_zlib
    rte_event_cnxk
    rte_mempool_ring
    rte_kvargs
    rte_gso
    rte_table
    rte_mbuf
    rte_dmadev
    rte_event_dpaa
    rte_net_fm10k
    rte_acl
    rte_pdump
    rte_distributor
    rte_crypto_null
    rte_ethdev
    rte_meter
    rte_common_ionic
    rte_net_vmxnet3
    rte_bus_dpaa
    rte_vhost
    rte_jobstats
    rte_net_qede
    rte_net_tap
    rte_common_idpf
    rte_net_hns3
    rte_common_cnxk
    rte_raw_dpaa2_cmdif
    rte_compress_nitrox
    rte_bus_fslmc
    rte_dma_dpaa
    rte_net_ixgbe
    rte_net_txgbe
    rte_eal
    rte_crypto_dpaa2_sec
    rte_common_nfp
    rte_common_nitrox
    rte_security
    rte_dma_ioat
    rte_hash
    rte_dispatcher
    rte_crypto_dpaa_sec
    )

# 添加源文件
aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC_LAZYBSD)

# 生成目标
add_library(${PROJECT_NAME} STATIC ${SRC_LAZYBSD})
target_compile_definitions(${PROJECT_NAME} PUBLIC LAZYBSD_USE_PAGE_ARRAY)

# 添加可执行文件
add_executable(${PROJECT_NAME}_runner main.cc)
target_link_libraries(${PROJECT_NAME}_runner PRIVATE
                      "$<LINK_GROUP:RESCAN,${PROJECT_NAME},freebsd,${DPDK_LIBRARIES}>")
target_link_options(${PROJECT_NAME}_runner
                    PRIVATE -Xlinker -Map=${PROJECT_NAME}_runner.map)

# 添加头文件路径
include_directories(${CMAKE_SOURCE_DIR}/include/generated)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/bsd/lib)

# 添加子文件夹
add_subdirectory(${CMAKE_SOURCE_DIR}/bsd ${CMAKE_BINARY_DIR}/bsd)
add_subdirectory(${CMAKE_SOURCE_DIR}/test ${CMAKE_BINARY_DIR}/test)
