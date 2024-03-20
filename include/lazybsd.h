/**
 * @file lazybsd.h
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-03
 *
 * @brief lazybsd根头文件
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#include <generated/config.h>
#include <generated/version.h>

#ifndef __LAZYBSD_H__
#define __LAZYBSD_H__

#ifdef __cplusplus
extern "C" {
#endif

// dpdk argc, argv, max argc: 16, member of dpdk_config
#define DPDK_CONFIG_NUM 			16
#define DPDK_CONFIG_MAXLEN 			256
#define DPDK_MAX_LCORE 				128
#define PCAP_SNAP_MINLEN 			94
#define PCAP_SAVE_MINLEN 			(2<<22)

#define MAX_PKT_BURST 				32
#define BURST_TX_DRAIN_US 			100 /* TX drain every ~100us */

#define VIP_MAX_NUM 				64

/* exception path(KNI) type */
#define KNI_TYPE_KNI        		0
#define KNI_TYPE_VIRTIO     		1

#ifdef __cplusplus
}
#endif

#endif /* __LAZYBSD_H__ */
