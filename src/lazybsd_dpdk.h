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
#ifndef __LAZYBSD_DPDK__
#define __LAZYBSD_DPDK__

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

#ifdef __cplusplus
}
#endif

#endif /* __LAZYBSD_DPDK__ */
