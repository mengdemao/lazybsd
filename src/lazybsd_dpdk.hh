/**
 * @file lazybsd_dpdk.hh
 * @author mengdemao (mengdemao19951021@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LAZYBSD_DPDK_HH
#define LAZYBSD_DPDK_HH
#include <string>
#include <string_view>
#include "lazybsd_dpdk.h"

int lazybsd_enable_pcap(std::string_view dump_path, uint16_t snap_len);
int lazybsd_dump_packets(std::string_view dump_path, struct rte_mbuf *pkt, uint16_t snap_len, uint32_t f_maxlen);

#endif // LAZYBSD_DPDK_HH