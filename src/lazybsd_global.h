/**
 * @file lazybsd_global.h
 * @author Meng Demao (mengdemao19951021@163.com)
 * @brief lazybsd_global管理
 * @version 0.1
 * @date 2024-12-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __LAZYBSD_GLOBAL_H__
#define __LAZYBSD_GLOBAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#define MAC_SIZE (6)

struct lazybsd_hw_features {
    uint8_t rx_csum;
    uint8_t rx_lro;
    uint8_t tx_csum_ip;
    uint8_t tx_csum_l4;
    uint8_t tx_tso;
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
    char *name;
    char *str;
    void *value;
    size_t vlen;
    struct lazybsd_freebsd_cfg *next;
};

enum {
    PORT_PRIME,
    PORT_PROXY,
    PORT_THIRD,
    PORT_SIZE
};

// dpdk argc, argv, max argc: 16, member of dpdk_config
#define DPDK_CONFIG_NUM 16
#define DPDK_CONFIG_MAXLEN 256
#define DPDK_MAX_LCORE 128
#define PCAP_SNAP_MINLEN 94
#define PCAP_SAVE_MINLEN (2<<22)

extern int dpdk_argc;
extern char *dpdk_argv[DPDK_CONFIG_NUM + 1];

#define MAX_PKT_BURST 32
#define BURST_TX_DRAIN_US 100 /* TX drain every ~100us */

#define VIP_MAX_NUM 64

/* exception path(KNI) type */
#define KNI_TYPE_KNI        0
#define KNI_TYPE_VIRTIO     1

struct lazybsd_port_cfg {
    char *name;
    char *ifname;
    uint8_t port_id;
    uint8_t mac[6];
    struct lazybsd_hw_features hw_features;
    const char *addr;
    const char *netmask;
    const char *broadcast;
    const char *gateway;

    char *vip_ifname;
    char *vip_addr_str;
    char **vip_addr_array;
    uint32_t nb_vip;

#ifdef INET6
    char *addr6_str;
    char *gateway6_str;
    uint8_t prefix_len;

    char *vip_addr6_str;
    char **vip_addr6_array;
    uint32_t nb_vip6;
    uint8_t vip_prefix_len;
#endif

    int nb_lcores;
    int nb_slaves;
    uint16_t lcore_list[DPDK_MAX_LCORE];
    uint16_t *slave_portid_list;
};

struct lazybsd_vdev_cfg {
    char *name;
    char *iface;
    char *path;
    char *mac;
    uint8_t vdev_id;
    uint8_t nb_queues;
    uint8_t nb_cq;
    uint16_t queue_size;
};

typedef struct {
    /* 调试开关 */ 
    bool debug;
         
    char config_filename[PATH_MAX];  /* config file name */
    char script_filename[PATH_MAX];  /* script file name */
    struct {
        char *proc_type;
        
        /* mask of enabled lcores */
        const char *lcore_mask;
        
        /* mask of current proc on all lcores */
        char *proc_mask;

        /* specify base virtual address to map. */
        char *base_virtaddr;

        /* allow processes that do not want to co-operate to have different memory regions */
        char *file_prefix;

        /* load an external driver */
        char *pci_whitelist;

        int nb_channel;
        int memory;
        int no_huge;
        int nb_procs;
        int proc_id;
        int promiscuous;
        int nb_vdev;
        int nb_bond;
        int numa_on;
        int tso;
        int tx_csum_offoad_skip;
        int vlan_strip;
        int symmetric_rss;

        /* sleep x microseconds when no pkts incomming */
        unsigned idle_sleep;

        /* TX burst queue drain nodelay dalay time */
        unsigned pkt_tx_delay;

        /* list of proc-lcore */
        uint16_t *proc_lcore;

        int nb_ports;
        uint16_t max_portid;
        uint16_t *portid_list;

        // load dpdk log level
        uint16_t log_level;
        // MAP(portid => struct lazybsd_port_cfg*)
        struct lazybsd_port_cfg *port_cfgs;
        struct lazybsd_vdev_cfg *vdev_cfgs;
        struct lazybsd_bond_cfg *bond_cfgs;
    } dpdk;

    struct {
        int enable;
        int type;
        char *kni_action;
        char *method;
        char *tcp_port;
        char *udp_port;
    } kni;

    struct {
        int level;
        const char *dir;
    } log;

    struct {
        struct lazybsd_freebsd_cfg *boot;
        struct lazybsd_freebsd_cfg *sysctl;
        long physmem;
        int hz;
        int fd_reserve;
        int mem_size;
    } freebsd;

    struct {
        uint16_t    enable;
        uint16_t    snap_len;
        uint32_t    save_len;
        const char* save_path;
    } pcap;

    /* luajit state */
    void *L;
} lazybsd_global;

/**
 * @brief get lazybsd_global_data ptr
 * 
 * @return lazybsd_global* 
 */
extern lazybsd_global* lazybsd_global_ptr(void);

/**
 * @brief print lazybsd global value
 * 
 */
void lazybsd_global_print(void);

#ifdef __cplusplus
}
#endif

#endif /* __LAZYBSD_GLOBAL_H__ */ 