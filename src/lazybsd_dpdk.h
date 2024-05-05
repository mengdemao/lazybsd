/**
 * @file lazybsd_dpdk.h
 * @author mengdemao (mengdemao19951021@163.com)
 * @brief
 * @version 0.1
 * @date 2024-03-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef LAZYBSD_DPDK_H
#define LAZYBSD_DPDK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lazybsd_api.h"

struct lazybsd_tx_offload {
    uint8_t ip_csum;
    uint8_t tcp_csum;
    uint8_t udp_csum;
    uint8_t sctp_csum;
    uint16_t tso_seg_size;
};

#define LAZYBSD_IF_NAME "lazybsd-%d"

struct loop_routine {
    loop_func_t loop;
    void *arg;
};

extern int enable_kni;
extern int nb_dev_ports;

enum FilterReturn {
    FILTER_UNKNOWN = -1,
    FILTER_ARP = 1,
    FILTER_KNI = 2,
#ifdef INET6
    FILTER_NDP = 3,  // Neighbor Solicitation/Advertisement, Router Solicitation/Advertisement/Redirect
#endif
};

int lazybsd_dpdk_init(int argc, char **argv);
int lazybsd_dpdk_if_up(void);
void lazybsd_dpdk_run(loop_func_t loop, void *arg);

struct lazybsd_dpdk_if_context;
struct lazybsd_port_cfg;

struct lazybsd_dpdk_if_context *lazybsd_dpdk_register_if(void *sc, void *ifp,
                                               struct lazybsd_port_cfg *cfg);
void lazybsd_dpdk_deregister_if(struct lazybsd_dpdk_if_context *ctx);

void lazybsd_dpdk_set_if(struct lazybsd_dpdk_if_context *ctx, void *sc, void *ifp);

int lazybsd_dpdk_if_send(struct lazybsd_dpdk_if_context* ctx, void *buf, int total);

void lazybsd_dpdk_pktmbuf_free(void *m);

void lazybsd_kni_init(uint16_t nb_ports, int type, const char *tcp_ports,
    const char *udp_ports);

void lazybsd_kni_alloc(uint16_t port_id, unsigned socket_id, int type, int port_idx,
    struct rte_mempool *mbuf_pool, unsigned ring_queue_size);

void lazybsd_kni_process(uint16_t port_id, uint16_t queue_id,
    struct rte_mbuf **pkts_burst, unsigned count);

enum FilterReturn lazybsd_kni_proto_filter(const void *data, uint16_t len, uint16_t eth_frame_type);

int lazybsd_kni_enqueue(uint16_t port_id, struct rte_mbuf *pkt);

#ifdef __cplusplus
}
#endif

#endif /* LAZYBSD_DPDK_H */
