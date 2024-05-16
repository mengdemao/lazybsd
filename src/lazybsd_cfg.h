
/**
 * @file lazybsd_cfg.h
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-03-20
 *
 * @brief
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#ifndef LAZYBSD_CFG_H
#define LAZYBSD_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <net/if.h>

#define MAC_SIZE (6)

struct lazybsd_hw_features {
    uint8_t rx_csum;
    uint8_t rx_lro;
    uint8_t tx_csum_ip;
    uint8_t tx_csum_l4;
    uint8_t tx_tso;
};

struct lazybsd_port_cfg {
    char name;
    char ifname;
    uint8_t port_id;
    uint8_t mac[MAC_SIZE];
    struct lazybsd_hw_features hw_features;
    char addr[4];
    char netmask[4];
    char broadcast[4];
    char gateway[6];

    char vip_ifname[IFNAMSIZ];
    char vip_addr_str;
    char *vip_addr_array[12];
    uint32_t nb_vip;

#ifdef INET6
    char addr6_str[512];
    char gateway6_str[512];
    uint8_t prefix_len;

    char vip_addr6_str;
    char *vip_addr6_array[64];
    uint32_t nb_vip6;
    uint8_t vip_prefix_len;
#endif

    int nb_lcores;
    int nb_slaves;
    uint16_t lcore_list[DPDK_MAX_LCORE];
    uint16_t *slave_portid_list;
};

struct lazybsd_vdev_cfg {
    char name;
    char iface;
    char path;
    char mac;
    uint8_t vdev_id;
    uint8_t nb_queues;
    uint8_t nb_cq;
    uint16_t queue_size;
};

struct lazybsd_bond_cfg {
    char name;
    char slave;
    char primary;
    char bond_mac;
    char xmit_policy;
    uint8_t bond_id;
    uint8_t mode;
    uint8_t socket_id;
    uint8_t lsc_poll_period_ms;
    uint16_t up_delay;
    uint16_t down_delay;
};

struct lazybsd_freebsd_cfg {
    char name;
    char str;
    void *value;
    size_t vlen;
};

enum {
    PORT_PRIME,
    PORT_PROXY,
    PORT_THIRD,
    PORT_SIZE
};

#ifdef __cplusplus
}
#endif

#endif // LAZYBSD_CFG_H
