/**
 * @file lazybsd_memory.cc
 * @author mengdemao (mengdemao19951021@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-04-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef LAZYBSD_MEMORY_H
#define LAZYBSD_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#define MEMPOOL_CACHE_SIZE 256

#define DISPATCH_RING_SIZE 2048

#define MSG_RING_SIZE 32

#define NB_SOCKETS 8

/*
 * Configurable number of RX/TX ring descriptors
 */
#define RX_QUEUE_SIZE 512
#define TX_QUEUE_SIZE 512

/*
 * Try to avoid TX buffering if we have at least MAX_TX_BURST packets to send.
 */
#define MAX_TX_BURST    (MAX_PKT_BURST / 2)

/* Configure how many packets ahead to prefetch, when reading packets */
#define PREFETCH_OFFSET    3

#define MAX_RX_QUEUE_PER_LCORE 16
#define MAX_TX_QUEUE_PER_PORT RTE_MAX_ETHPORTS
#define MAX_RX_QUEUE_PER_PORT 128

struct lazybsd_dpdk_if_context {
    void *sc;
    void *ifp;
    uint16_t port_id;
    struct lazybsd_hw_features hw_features;
} __rte_cache_aligned;

struct mbuf_table {
    uint16_t len;
    struct rte_mbuf *m_table[MAX_PKT_BURST];
#ifdef LAZYBSD_USE_PAGE_ARRAY
    void*            bsd_m_table[MAX_PKT_BURST];            // save bsd mbuf address which will be enquene into txring after NIC transmitted pkt.
#endif
};

struct lcore_rx_queue {
    uint16_t port_id;
    uint16_t queue_id;
} __rte_cache_aligned;

struct lcore_conf {
    uint16_t proc_id;
    uint16_t socket_id;
    uint16_t nb_queue_list[RTE_MAX_ETHPORTS];
    struct lazybsd_port_cfg *port_cfgs;

    uint16_t nb_rx_queue;
    struct lcore_rx_queue rx_queue_list[MAX_RX_QUEUE_PER_LCORE];
    uint16_t nb_tx_port;
    uint16_t tx_port_id[RTE_MAX_ETHPORTS];
    uint16_t tx_queue_id[RTE_MAX_ETHPORTS];
    struct mbuf_table tx_mbufs[RTE_MAX_ETHPORTS];
    //char *pcap[RTE_MAX_ETHPORTS];
} __rte_cache_aligned;

#ifdef LAZYBSD_USE_PAGE_ARRAY
//  mbuf_txring save mbuf which had bursted into NIC,  m_tables has same length with NIC dev's sw_ring.
//  Then when txring.m_table[x] is reused, the packet in txring.m_table[x] had been transmited by NIC.
//  that means the mbuf can be freed safely.
struct mbuf_txring{
    void* m_table[TX_QUEUE_SIZE];
    uint16_t head;        // next available element.
};

void lazybsd_init_ref_pool(int nb_mbuf, int socketid);
int lazybsd_mmap_init();
int lazybsd_if_send_onepkt(struct lazybsd_dpdk_if_context *ctx, void *m, int total);
int lazybsd_enq_tx_bsdmbuf(uint8_t portid, void *p_mbuf, int nb_segs);
#endif

extern struct rte_mempool *pktmbuf_pool[NB_SOCKETS];
extern struct lcore_conf lcore_conf;

#ifdef __cplusplus
}
#endif

#endif /* LAZYBSD_MEMORY_H */


