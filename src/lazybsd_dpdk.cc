/**
 * @file lazybsd_dpdk.cc
 * @author mengdemao (mengdemao19951021@163.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <rte_common.h>
#include <rte_byteorder.h>
#include <rte_log.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_memzone.h>
#include <rte_config.h>
#include <rte_eal.h>
#include <rte_pci.h>
#include <rte_mbuf.h>
#include <rte_memory.h>
#include <rte_lcore.h>
#include <rte_launch.h>
#include <rte_ethdev.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_malloc.h>
#include <rte_cycles.h>
#include <rte_timer.h>
#include <rte_thash.h>
#include <rte_ip.h>
#include <rte_tcp.h>
#include <rte_udp.h>
#include <rte_eth_bond.h>
#include <rte_eth_bond_8023ad.h>

#include <lazybsd_cfg.hh>
#include <lazybsd_memory.hh>
#include <lazybsd_api.hh>
#include <lazybsd_msg.hh>
#include <lazybsd_host.hh>
#include <lazybsd_dpdk.hh>
#include <lazybsd_veth.hh>
#include <lazybsd_socket.hh>

#include <iostream>
#include <string>

namespace lazybsd {
namespace dpdk {

#ifdef LAZYBSD_KNI
#define KNI_MBUF_MAX 2048
#define KNI_QUEUE_SIZE KNI_MBUF_MAX

int enable_kni = 0;
static int kni_accept;
static int knictl_action = LAZYBSD_KNICTL_ACTION_DEFAULT;
#endif // LAZYBSD_KNI

int nb_dev_ports = 0;   /* primary is correct, secondary is not correct, but no impact now*/

static int numa_on;

static unsigned idle_sleep;
static unsigned pkt_tx_delay;
static uint64_t usr_cb_tsc;

static struct rte_timer freebsd_clock;

// Mellanox Linux's driver key
static uint8_t default_rsskey_40bytes[40] = {
    0xd1, 0x81, 0xc6, 0x2c, 0xf7, 0xf4, 0xdb, 0x5b,
    0x19, 0x83, 0xa2, 0xfc, 0x94, 0x3e, 0x1a, 0xdb,
    0xd9, 0x38, 0x9e, 0x6b, 0xd1, 0x03, 0x9c, 0x2c,
    0xa7, 0x44, 0x99, 0xad, 0x59, 0x3d, 0x56, 0xd9,
    0xf3, 0x25, 0x3c, 0x06, 0x2a, 0xdc, 0x1f, 0xfc
};

static uint8_t default_rsskey_52bytes[52] = {
    0x44, 0x39, 0x79, 0x6b, 0xb5, 0x4c, 0x50, 0x23,
    0xb6, 0x75, 0xea, 0x5b, 0x12, 0x4f, 0x9f, 0x30,
    0xb8, 0xa2, 0xc0, 0x3d, 0xdf, 0xdc, 0x4d, 0x02,
    0xa0, 0x8c, 0x9b, 0x33, 0x4a, 0xf6, 0x4a, 0x4c,
    0x05, 0xc6, 0xfa, 0x34, 0x39, 0x58, 0xd8, 0x55,
    0x7d, 0x99, 0x58, 0x3a, 0xe1, 0x38, 0xc9, 0x2e,
    0x81, 0x15, 0x03, 0x66
};

static uint8_t symmetric_rsskey[52] = {
    0x6d, 0x5a, 0x6d, 0x5a, 0x6d, 0x5a, 0x6d, 0x5a,
    0x6d, 0x5a, 0x6d, 0x5a, 0x6d, 0x5a, 0x6d, 0x5a,
    0x6d, 0x5a, 0x6d, 0x5a, 0x6d, 0x5a, 0x6d, 0x5a,
    0x6d, 0x5a, 0x6d, 0x5a, 0x6d, 0x5a, 0x6d, 0x5a,
    0x6d, 0x5a, 0x6d, 0x5a, 0x6d, 0x5a, 0x6d, 0x5a,
    0x6d, 0x5a, 0x6d, 0x5a, 0x6d, 0x5a, 0x6d, 0x5a,
    0x6d, 0x5a, 0x6d, 0x5a
};

static int rsskey_len = sizeof(default_rsskey_40bytes);
static uint8_t *rsskey = default_rsskey_40bytes;

struct lcore_conf lcore_conf;

struct rte_mempool *pktmbuf_pool[NB_SOCKETS];

static pcblddr_func_t pcblddr_fun;

static struct rte_ring **dispatch_ring[RTE_MAX_ETHPORTS];
static dispatch_func_t packet_dispatcher;

static uint16_t rss_reta_size[RTE_MAX_ETHPORTS];

#define BOND_DRIVER_NAME    "net_bonding"

static inline int send_single_packet(struct rte_mbuf *m, uint8_t port);

struct lazybsd_msg_ring {
    char ring_name[LAZYBSD_MSG_NUM][RTE_RING_NAMESIZE];
    /* ring[0] for lcore recv msg, other send */
    /* ring[1] for lcore send msg, other read */
    struct rte_ring *ring[LAZYBSD_MSG_NUM];
} __rte_cache_aligned;

static struct lazybsd_msg_ring msg_ring[RTE_MAX_LCORE];
static struct rte_mempool *message_pool;
static struct lazybsd_dpdk_if_context *veth_ctx[RTE_MAX_ETHPORTS];

static struct lazybsd_top_args lazybsd_top_status;
static struct lazybsd_traffic_args lazybsd_traffic;
extern void lazybsd_hardclock(void);

/* Callback for request of changing MTU */
/* Total octets in ethernet header */
#define KNI_ENET_HEADER_SIZE    14

/* Total octets in the FCS */
#define KNI_ENET_FCS_SIZE       4

#ifndef RTE_KNI_NAMESIZE
#define RTE_KNI_NAMESIZE 16
#endif

#define set_bit(n, m)   (n | magic_bits[m])
#define clear_bit(n, m) (n & (~magic_bits[m]))
#define get_bit(n, m)   (n & magic_bits[m])

static const int magic_bits[8] = {
    0x80, 0x40, 0x20, 0x10,
    0x8, 0x4, 0x2, 0x1
};

static unsigned char *udp_port_bitmap = NULL;
static unsigned char *tcp_port_bitmap = NULL;

/* Structure type for recording kni interface specific stats */
struct kni_interface_stats {
#ifdef LAZYBSD_KNI_KNI
    struct rte_kni *kni;
#endif

    /* port id of dev or virtio_user */
    uint16_t port_id;

    /* number of pkts received from NIC, and sent to KNI */
    uint64_t rx_packets;

    /* number of pkts received from NIC, but failed to send to KNI */
    uint64_t rx_dropped;

    /* number of pkts received from KNI, and sent to NIC */
    uint64_t tx_packets;

    /* number of pkts received from KNI, but failed to send to NIC */
    uint64_t tx_dropped;
};

struct rte_ring **kni_rp;
struct kni_interface_stats **kni_stat;

#define FILE_PATH_LEN 64
#define PCAP_FILE_NUM 10

struct pcap_file_header {
    uint32_t magic;
    u_short version_major;
    u_short version_minor;
    int32_t thiszone;        /* gmt to local correction */
    uint32_t sigfigs;        /* accuracy of timestamps */
    uint32_t snaplen;        /* max length saved portion of each pkt */
    uint32_t linktype;       /* data link type (LINKTYPE_*) */
};

struct pcap_pkthdr {
    uint32_t sec;            /* time stamp */
    uint32_t usec;           /* struct timeval time_t, in linux64: 8*2=16, in cap: 4 */
    uint32_t caplen;         /* length of portion present */
    uint32_t len;            /* length this packet (off wire) */
};

static __thread FILE* g_pcap_fp = NULL;
static __thread uint32_t seq = 0;
static __thread uint32_t g_flen = 0;

static void lazybsd_hardclock_job(__rte_unused struct rte_timer *timer,
    __rte_unused void *arg) {
    lazybsd_hardclock();
    lazybsd_update_current_ts();
}

struct lazybsd_dpdk_if_context *
lazybsd_dpdk_register_if(void *sc, void *ifp, struct lazybsd_port_cfg *cfg)
{
    struct lazybsd_dpdk_if_context *ctx;

    ctx = (struct lazybsd_dpdk_if_context *)calloc(1, sizeof(struct lazybsd_dpdk_if_context));
    if (ctx == NULL)
        return NULL;

    ctx->sc = sc;
    ctx->ifp = ifp;
    ctx->port_id = cfg->port_id;
    ctx->hw_features = cfg->hw_features;

    return ctx;
}

void
lazybsd_dpdk_deregister_if(struct lazybsd_dpdk_if_context *ctx)
{
    free(ctx);
}

static void
check_all_ports_link_status(void)
{
    #define CHECK_INTERVAL 100 /* 100ms */
    #define MAX_CHECK_TIME 90  /* 9s (90 * 100ms) in total */

    uint16_t portid;
    uint8_t count, all_ports_up, print_flag = 0;
    struct rte_eth_link link;

    printf("\nChecking link status");
    fflush(stdout);

    int i, nb_ports;
    nb_ports = lazybsd_global_cfg.dpdk.nb_ports;
    for (count = 0; count <= MAX_CHECK_TIME; count++) {
        all_ports_up = 1;
        for (i = 0; i < nb_ports; i++) {
            uint16_t portid = lazybsd_global_cfg.dpdk.portid_list[i];
            memset(&link, 0, sizeof(link));
            rte_eth_link_get_nowait(portid, &link);

            /* print link status if flag set */
            if (print_flag == 1) {
                if (link.link_status) {
                    printf("Port %d Link Up - speed %u "
                        "Mbps - %s\n", (int)portid,
                        (unsigned)link.link_speed,
                        (link.link_duplex == RTE_ETH_LINK_FULL_DUPLEX) ?
                        ("full-duplex") : ("half-duplex\n"));
                } else {
                    printf("Port %d Link Down\n", (int)portid);
                }
                continue;
            }
            /* clear all_ports_up flag if any link down */
            if (link.link_status == 0) {
                all_ports_up = 0;
                break;
            }
        }

        /* after finally printing all link status, get out */
        if (print_flag == 1)
            break;

        if (all_ports_up == 0) {
            printf(".");
            fflush(stdout);
            rte_delay_ms(CHECK_INTERVAL);
        }

        /* set the print_flag if all ports up or timeout */
        if (all_ports_up == 1 || count == (MAX_CHECK_TIME - 1)) {
            print_flag = 1;
            printf("done\n");
        }
    }
}

static int
init_lcore_conf(void)
{
    if (nb_dev_ports == 0) {
        nb_dev_ports = rte_eth_dev_count_avail();
    }
    if (nb_dev_ports == 0) {
        rte_exit(EXIT_FAILURE, "No probed ethernet devices\n");
    }

    if (lazybsd_global_cfg.dpdk.max_portid >= nb_dev_ports) {
        rte_exit(EXIT_FAILURE, "this machine doesn't have port %d.\n",
                 lazybsd_global_cfg.dpdk.max_portid);
    }

    lcore_conf.port_cfgs = lazybsd_global_cfg.dpdk.port_cfgs;
    lcore_conf.proc_id = lazybsd_global_cfg.dpdk.proc_id;

    uint16_t socket_id = 0;
    if (numa_on) {
        socket_id = rte_lcore_to_socket_id(rte_lcore_id());
    }

    lcore_conf.socket_id = socket_id;

    uint16_t lcore_id = lazybsd_global_cfg.dpdk.proc_lcore[lcore_conf.proc_id];
    if (!rte_lcore_is_enabled(lcore_id)) {
        rte_exit(EXIT_FAILURE, "lcore %u unavailable\n", lcore_id);
    }

    int j;
    for (j = 0; j < lazybsd_global_cfg.dpdk.nb_ports; ++j) {
        uint16_t port_id = lazybsd_global_cfg.dpdk.portid_list[j];
        struct lazybsd_port_cfg *pconf = &lazybsd_global_cfg.dpdk.port_cfgs[port_id];

        int queueid = -1;
        int i;
        for (i = 0; i < pconf->nb_lcores; i++) {
            if (pconf->lcore_list[i] == lcore_id) {
                queueid = i;
            }
        }
        if (queueid < 0) {
            continue;
        }
        printf("lcore: %u, port: %u, queue: %u\n", lcore_id, port_id, queueid);
        uint16_t nb_rx_queue = lcore_conf.nb_rx_queue;
        lcore_conf.rx_queue_list[nb_rx_queue].port_id = port_id;
        lcore_conf.rx_queue_list[nb_rx_queue].queue_id = queueid;
        lcore_conf.nb_rx_queue++;

        lcore_conf.tx_queue_id[port_id] = queueid;
        lcore_conf.tx_port_id[lcore_conf.nb_tx_port] = port_id;
        lcore_conf.nb_tx_port++;

        /* Enable pcap dump */
        if (lazybsd_global_cfg.pcap.enable) {
            lazybsd_enable_pcap(lazybsd_global_cfg.pcap.save_path, lazybsd_global_cfg.pcap.snap_len);
        }

        lcore_conf.nb_queue_list[port_id] = pconf->nb_lcores;
    }

    if (lcore_conf.nb_rx_queue == 0) {
        rte_exit(EXIT_FAILURE, "lcore %u has nothing to do\n", lcore_id);
    }

    return 0;
}

static int
init_mem_pool(void)
{
    uint8_t nb_ports = lazybsd_global_cfg.dpdk.nb_ports;
    uint32_t nb_lcores = lazybsd_global_cfg.dpdk.nb_procs;
    uint32_t nb_tx_queue = nb_lcores;
    uint32_t nb_rx_queue = lcore_conf.nb_rx_queue * nb_lcores;
    uint16_t max_portid = lazybsd_global_cfg.dpdk.max_portid;

    unsigned nb_mbuf = RTE_ALIGN_CEIL (
        (nb_rx_queue * (max_portid + 1) * 2 * RX_QUEUE_SIZE          +
        nb_ports * (max_portid + 1) * 2 * nb_lcores * MAX_PKT_BURST    +
        nb_ports * (max_portid + 1) * 2 * nb_tx_queue * TX_QUEUE_SIZE  +
        nb_lcores * MEMPOOL_CACHE_SIZE +
#ifdef LAZYBSD_KNI
        nb_ports * KNI_MBUF_MAX +
        nb_ports * KNI_QUEUE_SIZE +
#endif
        nb_lcores * nb_ports * DISPATCH_RING_SIZE),
        (unsigned)8192);

    unsigned socketid = 0;
    uint16_t i, lcore_id;
    char s[64];

    for (i = 0; i < lazybsd_global_cfg.dpdk.nb_procs; i++) {
        lcore_id = lazybsd_global_cfg.dpdk.proc_lcore[i];
        if (numa_on) {
            socketid = rte_lcore_to_socket_id(lcore_id);
        }

        if (socketid >= NB_SOCKETS) {
            rte_exit(EXIT_FAILURE, "Socket %d of lcore %u is out of range %d\n",
                socketid, i, NB_SOCKETS);
        }

        if (pktmbuf_pool[socketid] != NULL) {
            continue;
        }

        if (rte_eal_process_type() == RTE_PROC_PRIMARY) {
            snprintf(s, sizeof(s), "mbuf_pool_%d", socketid);
            pktmbuf_pool[socketid] =
                rte_pktmbuf_pool_create(s, nb_mbuf,
                    MEMPOOL_CACHE_SIZE, 0,
                    RTE_MBUF_DEFAULT_BUF_SIZE, socketid);
        } else {
            snprintf(s, sizeof(s), "mbuf_pool_%d", socketid);
            pktmbuf_pool[socketid] = rte_mempool_lookup(s);
        }

        if (pktmbuf_pool[socketid] == NULL) {
            rte_exit(EXIT_FAILURE, "Cannot create mbuf pool on socket %d\n", socketid);
        } else {
            printf("create mbuf pool on socket %d\n", socketid);
        }

#ifdef LAZYBSD_USE_PAGE_ARRAY
        nb_mbuf = RTE_ALIGN_CEIL (
            nb_ports*nb_lcores*MAX_PKT_BURST    +
            nb_ports*nb_tx_queue*TX_QUEUE_SIZE  +
            nb_lcores*MEMPOOL_CACHE_SIZE,
            (unsigned)4096);
        lazybsd_init_ref_pool(nb_mbuf, socketid);
#endif
    }

    return 0;
}

static struct rte_ring *
create_ring(const char *name, unsigned count, int socket_id, unsigned flags)
{
    struct rte_ring *ring;

    if (name == NULL) {
        rte_exit(EXIT_FAILURE, "create ring failed, no name!\n");
    }

    if (rte_eal_process_type() == RTE_PROC_PRIMARY) {
        ring = rte_ring_create(name, count, socket_id, flags);
    } else {
        ring = rte_ring_lookup(name);
    }

    if (ring == NULL) {
        rte_exit(EXIT_FAILURE, "create ring:%s failed!\n", name);
    }

    return ring;
}

static int
init_dispatch_ring(void)
{
    int j;
    char name_buf[RTE_RING_NAMESIZE];
    int queueid;

    unsigned socketid = lcore_conf.socket_id;

    /* Create ring according to ports actually being used. */
    int nb_ports = lazybsd_global_cfg.dpdk.nb_ports;
    for (j = 0; j < nb_ports; j++) {
        uint16_t portid = lazybsd_global_cfg.dpdk.portid_list[j];
        struct lazybsd_port_cfg *pconf = &lazybsd_global_cfg.dpdk.port_cfgs[portid];
        int nb_queues = pconf->nb_lcores;
        if (dispatch_ring[portid] == NULL) {
            snprintf(name_buf, RTE_RING_NAMESIZE, "ring_ptr_p%d", portid);

            dispatch_ring[portid] = (struct rte_ring **)rte_zmalloc(name_buf,
                sizeof(struct rte_ring *) * nb_queues,
                RTE_CACHE_LINE_SIZE);
            if (dispatch_ring[portid] == NULL) {
                rte_exit(EXIT_FAILURE, "rte_zmalloc(%s (struct rte_ring*)) "
                    "failed\n", name_buf);
            }
        }

        for(queueid = 0; queueid < nb_queues; ++queueid) {
            snprintf(name_buf, RTE_RING_NAMESIZE, "dispatch_ring_p%d_q%d",
                portid, queueid);
            dispatch_ring[portid][queueid] = create_ring(name_buf,
                DISPATCH_RING_SIZE, socketid, RING_F_SC_DEQ);

            if (dispatch_ring[portid][queueid] == NULL)
                rte_panic("create ring:%s failed!\n", name_buf);

            printf("create ring:%s success, %u ring entries are now free!\n",
                name_buf, rte_ring_free_count(dispatch_ring[portid][queueid]));
        }
    }

    return 0;
}

static void
lazybsd_msg_init(struct rte_mempool *mp,
    __attribute__((unused)) void *opaque_arg,
    void *obj, __attribute__((unused)) unsigned i)
{
    struct lazybsd_msg *msg = (struct lazybsd_msg *)obj;
    msg->msg_type = LAZYBSD_UNKNOWN;
    msg->buf_addr = (char *)msg + sizeof(struct lazybsd_msg);
    msg->buf_len = mp->elt_size - sizeof(struct lazybsd_msg);
    msg->original_buf = NULL;
    msg->original_buf_len = 0;
}

static int
init_msg_ring(void)
{
    uint16_t i, j;
    uint16_t nb_procs = lazybsd_global_cfg.dpdk.nb_procs;
    unsigned socketid = lcore_conf.socket_id;

    /* Create message buffer pool */
    if (rte_eal_process_type() == RTE_PROC_PRIMARY) {
        message_pool = rte_mempool_create(LAZYBSD_MSG_POOL,
           MSG_RING_SIZE * 2 * nb_procs,
           MAX_MSG_BUF_SIZE, MSG_RING_SIZE / 2, 0,
           NULL, NULL, lazybsd_msg_init, NULL,
           socketid, 0);
    } else {
        message_pool = rte_mempool_lookup(LAZYBSD_MSG_POOL);
    }

    if (message_pool == NULL) {
        rte_panic("Create msg mempool failed\n");
    }

    for(i = 0; i < nb_procs; ++i) {
        snprintf(msg_ring[i].ring_name[0], RTE_RING_NAMESIZE,
            "%s%u", LAZYBSD_MSG_RING_IN, i);
        msg_ring[i].ring[0] = create_ring(msg_ring[i].ring_name[0],
            MSG_RING_SIZE, socketid, RING_F_SP_ENQ | RING_F_SC_DEQ);
        if (msg_ring[i].ring[0] == NULL)
            rte_panic("create ring::%s failed!\n", msg_ring[i].ring_name[0]);

        for (j = LAZYBSD_SYSCTL; j < LAZYBSD_MSG_NUM; j++) {
            snprintf(msg_ring[i].ring_name[j], RTE_RING_NAMESIZE,
                "%s%u_%u", LAZYBSD_MSG_RING_OUT, i, j);
            msg_ring[i].ring[j] = create_ring(msg_ring[i].ring_name[j],
                MSG_RING_SIZE, socketid, RING_F_SP_ENQ | RING_F_SC_DEQ);
            if (msg_ring[i].ring[j] == NULL)
                rte_panic("create ring::%s failed!\n", msg_ring[i].ring_name[j]);
        }
    }

    return 0;
}

#ifdef LAZYBSD_KNI

static enum LAZYBSD_KNICTL_CMD get_kni_action(const char *c){
    if (!c)
        return LAZYBSD_KNICTL_ACTION_DEFAULT;
    if (0 == strcasecmp(c, "alltokni")){
        return LAZYBSD_KNICTL_ACTION_ALL_TO_KNI;
    } else  if (0 == strcasecmp(c, "alltoff")){
        return LAZYBSD_KNICTL_ACTION_ALL_TO_FF;
    } else if (0 == strcasecmp(c, "default")){
        return LAZYBSD_KNICTL_ACTION_DEFAULT;
    } else {
        return LAZYBSD_KNICTL_ACTION_DEFAULT;
    }
}

static int
init_kni(void)
{
    int nb_ports = nb_dev_ports;

    kni_accept = 0;

    if(strcasecmp(lazybsd_global_cfg.kni.method, "accept") == 0)
        kni_accept = 1;

    knictl_action = get_kni_action(lazybsd_global_cfg.kni.kni_action);

    lazybsd_kni_init(nb_ports, lazybsd_global_cfg.kni.type, lazybsd_global_cfg.kni.tcp_port,
        lazybsd_global_cfg.kni.udp_port);

    unsigned socket_id = lcore_conf.socket_id;
    struct rte_mempool *mbuf_pool = pktmbuf_pool[socket_id];

    nb_ports = lazybsd_global_cfg.dpdk.nb_ports;
    int i, ret;
    for (i = 0; i < nb_ports; i++) {
        uint16_t port_id = lazybsd_global_cfg.dpdk.portid_list[i];
        lazybsd_kni_alloc(port_id, socket_id, lazybsd_global_cfg.kni.type, i, mbuf_pool, KNI_QUEUE_SIZE);
    }

    return 0;
}
#endif

//RSS reta update will failed when enable flow isolate
#ifndef LAZYBSD_FLOW_ISOLATE
static void
set_rss_table(uint16_t port_id, uint16_t reta_size, uint16_t nb_queues)
{
    if (reta_size == 0) {
        return;
    }

    int reta_conf_size = RTE_MAX(1, reta_size / RTE_ETH_RETA_GROUP_SIZE);
    struct rte_eth_rss_reta_entry64 reta_conf[reta_conf_size];

    /* config HW indirection table */
    unsigned i, j, hash=0;
    for (i = 0; i < reta_conf_size; i++) {
        reta_conf[i].mask = ~0ULL;
        for (j = 0; j < RTE_ETH_RETA_GROUP_SIZE; j++) {
            reta_conf[i].reta[j] = hash++ % nb_queues;
        }
    }

    if (rte_eth_dev_rss_reta_update(port_id, reta_conf, reta_size)) {
        rte_exit(EXIT_FAILURE, "port[%d], failed to update rss table\n",
            port_id);
    }
}
#endif

static int
init_port_start(void)
{
    int nb_ports = lazybsd_global_cfg.dpdk.nb_ports, total_nb_ports;
    unsigned socketid = 0;
    struct rte_mempool *mbuf_pool;
    uint16_t i, j;

    total_nb_ports = nb_ports;
#ifdef LAZYBSD_KNI
    if (enable_kni && rte_eal_process_type() == RTE_PROC_PRIMARY) {
#ifdef LAZYBSD_KNI_KNI
        if (lazybsd_global_cfg.kni.type == KNI_TYPE_VIRTIO)
#endif
        {
            total_nb_ports *= 2;  /* one more virtio_user port for kernel per port */
        }
    }
#endif
    for (i = 0; i < total_nb_ports; i++) {
        uint16_t port_id, u_port_id;
        struct lazybsd_port_cfg *pconf = NULL;
        uint16_t nb_queues;
        int nb_slaves;

        if (i < nb_ports) {
            u_port_id = lazybsd_global_cfg.dpdk.portid_list[i];
            pconf = &lazybsd_global_cfg.dpdk.port_cfgs[u_port_id];
            nb_queues = pconf->nb_lcores;
            nb_slaves = pconf->nb_slaves;

            if (nb_slaves > 0) {
                rte_eth_bond_8023ad_dedicated_queues_enable(u_port_id);
            }
        } else {
            /* kernel virtio user, port id start from `nb_dev_ports` */
            u_port_id = i - nb_ports + nb_dev_ports;
            nb_queues = 1; /* see lazybsd_kni_alloc in lazybsd_dpdk_kni.c */
            nb_slaves = 0;
        }


        for (j = 0; j <= nb_slaves; j++) {
            if (j < nb_slaves) {
                port_id = pconf->slave_portid_list[j];
                printf("To init %s's %d'st slave port[%d]\n",
                        lazybsd_global_cfg.dpdk.bond_cfgs->name,
                        j, port_id);
            } else {
                port_id = u_port_id;
            }

            struct rte_eth_dev_info dev_info;
            struct rte_eth_conf port_conf = {0};
            struct rte_eth_rxconf rxq_conf;
            struct rte_eth_txconf txq_conf;

            int ret = rte_eth_dev_info_get(port_id, &dev_info);
            if (ret != 0)
                rte_exit(EXIT_FAILURE,
                    "Error during getting device (port %u) info: %s\n",
                    port_id, strerror(-ret));

            if (nb_queues > dev_info.max_rx_queues) {
                rte_exit(EXIT_FAILURE, "num_procs[%d] bigger than max_rx_queues[%d]\n",
                    nb_queues,
                    dev_info.max_rx_queues);
            }

            if (nb_queues > dev_info.max_tx_queues) {
                rte_exit(EXIT_FAILURE, "num_procs[%d] bigger than max_tx_queues[%d]\n",
                    nb_queues,
                    dev_info.max_tx_queues);
            }

            struct rte_ether_addr addr;
            rte_eth_macaddr_get(port_id, &addr);
            printf("Port %u MAC:"RTE_ETHER_ADDR_PRT_FMT"\n",
                    (unsigned)port_id, RTE_ETHER_ADDR_BYTES(&addr));

            /* Only config dev port, but not kernel virtio user port */
            if (pconf) {
                rte_memcpy(pconf->mac,
                    addr.addr_bytes, RTE_ETHER_ADDR_LEN);

                /* Set RSS mode */
                uint64_t default_rss_hf = RTE_ETH_RSS_PROTO_MASK;
                port_conf.rxmode.mq_mode = RTE_ETH_MQ_RX_RSS;
                port_conf.rx_adv_conf.rss_conf.rss_hf = default_rss_hf;
                if (dev_info.hash_key_size == 52) {
                    rsskey = default_rsskey_52bytes;
                    rsskey_len = 52;
                }
                if (lazybsd_global_cfg.dpdk.symmetric_rss) {
                    printf("Use symmetric Receive-side Scaling(RSS) key\n");
                    rsskey = symmetric_rsskey;
                }
                port_conf.rx_adv_conf.rss_conf.rss_key = rsskey;
                port_conf.rx_adv_conf.rss_conf.rss_key_len = rsskey_len;
                port_conf.rx_adv_conf.rss_conf.rss_hf &= dev_info.flow_type_rss_offloads;
                if (port_conf.rx_adv_conf.rss_conf.rss_hf !=
                        RTE_ETH_RSS_PROTO_MASK) {
                    printf("Port %u modified RSS hash function based on hardware support,"
                            "requested:%#"PRIx64" configured:%#"PRIx64"\n",
                            port_id, default_rss_hf,
                            port_conf.rx_adv_conf.rss_conf.rss_hf);
                }

                if (dev_info.tx_offload_capa & RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE) {
                    port_conf.txmode.offloads |=
                        RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE;
                }

                /* Set Rx VLAN stripping */
                if (lazybsd_global_cfg.dpdk.vlan_strip) {
                    if (dev_info.rx_offload_capa & RTE_ETH_RX_OFFLOAD_VLAN_STRIP) {
                        port_conf.rxmode.offloads |= RTE_ETH_RX_OFFLOAD_VLAN_STRIP;
                    }
                }

                /* Enable HW CRC stripping */
                port_conf.rxmode.offloads &= ~RTE_ETH_RX_OFFLOAD_KEEP_CRC;

                /* FIXME: Enable TCP LRO ?*/
                #if 0
                if (dev_info.rx_offload_capa & DEV_RX_OFFLOAD_TCP_LRO) {
                    printf("LRO is supported\n");
                    port_conf.rxmode.offloads |= DEV_RX_OFFLOAD_TCP_LRO;
                    pconf->hw_features.rx_lro = 1;
                }
                #endif

                /* Set Rx checksum checking */
                if ((dev_info.rx_offload_capa & RTE_ETH_RX_OFFLOAD_IPV4_CKSUM) &&
                    (dev_info.rx_offload_capa & RTE_ETH_RX_OFFLOAD_UDP_CKSUM) &&
                    (dev_info.rx_offload_capa & RTE_ETH_RX_OFFLOAD_TCP_CKSUM)) {
                    printf("RX checksum offload supported\n");
                    port_conf.rxmode.offloads |= RTE_ETH_RX_OFFLOAD_CHECKSUM;
                    pconf->hw_features.rx_csum = 1;
                }

                if (lazybsd_global_cfg.dpdk.tx_csum_offoad_skip == 0) {
                    if ((dev_info.tx_offload_capa & RTE_ETH_TX_OFFLOAD_IPV4_CKSUM)) {
                        printf("TX ip checksum offload supported\n");
                        port_conf.txmode.offloads |= RTE_ETH_TX_OFFLOAD_IPV4_CKSUM;
                        pconf->hw_features.tx_csum_ip = 1;
                    }

                    if ((dev_info.tx_offload_capa & RTE_ETH_TX_OFFLOAD_UDP_CKSUM) &&
                        (dev_info.tx_offload_capa & RTE_ETH_TX_OFFLOAD_TCP_CKSUM)) {
                        printf("TX TCP&UDP checksum offload supported\n");
                        port_conf.txmode.offloads |= RTE_ETH_TX_OFFLOAD_UDP_CKSUM | RTE_ETH_TX_OFFLOAD_TCP_CKSUM;
                        pconf->hw_features.tx_csum_l4 = 1;
                    }
                } else {
                    printf("TX checksum offoad is disabled\n");
                }

                if (lazybsd_global_cfg.dpdk.tso) {
                    if (dev_info.tx_offload_capa & RTE_ETH_TX_OFFLOAD_TCP_TSO) {
                        printf("TSO is supported\n");
                        port_conf.txmode.offloads |= RTE_ETH_TX_OFFLOAD_TCP_TSO;
                        pconf->hw_features.tx_tso = 1;
                    }
                    else {
                        printf("TSO is not supported\n");
                    }
                } else {
                    printf("TSO is disabled\n");
                }

                if (dev_info.reta_size) {
                    /* reta size must be power of 2 */
                    assert((dev_info.reta_size & (dev_info.reta_size - 1)) == 0);

                    rss_reta_size[port_id] = dev_info.reta_size;
                    printf("port[%d]: rss table size: %d\n", port_id,
                        dev_info.reta_size);
                }
            }

            if (rte_eal_process_type() != RTE_PROC_PRIMARY) {
                continue;
            }

            ret = rte_eth_dev_configure(port_id, nb_queues, nb_queues, &port_conf);
            if (ret != 0) {
                return ret;
            }

            static uint16_t nb_rxd = RX_QUEUE_SIZE;
            static uint16_t nb_txd = TX_QUEUE_SIZE;
            ret = rte_eth_dev_adjust_nb_rx_tx_desc(port_id, &nb_rxd, &nb_txd);
            if (ret < 0)
                printf("Could not adjust number of descriptors "
                        "for port%u (%d)\n", (unsigned)port_id, ret);

            uint16_t q;
            for (q = 0; q < nb_queues; q++) {
                if (numa_on) {
                    uint16_t lcore_id = lcore_conf.port_cfgs[u_port_id].lcore_list[q];
                    socketid = rte_lcore_to_socket_id(lcore_id);
                }
                mbuf_pool = pktmbuf_pool[socketid];

                txq_conf = dev_info.default_txconf;
                txq_conf.offloads = port_conf.txmode.offloads;
                ret = rte_eth_tx_queue_setup(port_id, q, nb_txd,
                    socketid, &txq_conf);
                if (ret < 0) {
                    return ret;
                }

                rxq_conf = dev_info.default_rxconf;
                rxq_conf.offloads = port_conf.rxmode.offloads;
                ret = rte_eth_rx_queue_setup(port_id, q, nb_rxd,
                    socketid, &rxq_conf, mbuf_pool);
                if (ret < 0) {
                    return ret;
                }
            }

            if (strncmp(dev_info.driver_name, BOND_DRIVER_NAME,
                    strlen(dev_info.driver_name)) == 0) {

                rte_eth_macaddr_get(port_id, &addr);
                printf("Port %u MAC:"RTE_ETHER_ADDR_PRT_FMT"\n",
                        (unsigned)port_id, RTE_ETHER_ADDR_BYTES(&addr));

                rte_memcpy(pconf->mac,
                    addr.addr_bytes, RTE_ETHER_ADDR_LEN);

                int mode, count, x;
                uint16_t slaves[RTE_MAX_ETHPORTS], len = RTE_MAX_ETHPORTS;

                mode = rte_eth_bond_mode_get(port_id);
                printf("Port %u, bond mode:%d\n", port_id, mode);

                count = rte_eth_bond_members_get(port_id, slaves, len);
                printf("Port %u, %s's slave ports count:%d\n", port_id,
                            lazybsd_global_cfg.dpdk.bond_cfgs->name, count);
                for (x=0; x<count; x++) {
                    printf("Port %u, %s's slave port[%u]\n", port_id,
                            lazybsd_global_cfg.dpdk.bond_cfgs->name, slaves[x]);
                }
            }

            ret = rte_eth_dev_start(port_id);
            if (ret < 0) {
                return ret;
            }

//RSS reta update will failed when enable flow isolate
#ifndef LAZYBSD_FLOW_ISOLATE
            if (nb_queues > 1) {
                /*
                 * FIXME: modify RSS set to FDIR
                 */
                set_rss_table(port_id, dev_info.reta_size, nb_queues);
            }
#endif

            /* Enable RX in promiscuous mode for the Ethernet device. */
            if (lazybsd_global_cfg.dpdk.promiscuous) {
                ret = rte_eth_promiscuous_enable(port_id);
                if (ret == 0) {
                    printf("set port %u to promiscuous mode ok\n", port_id);
                } else {
                    printf("set port %u to promiscuous mode error\n", port_id);
                }
            }
        }
    }

    if (rte_eal_process_type() == RTE_PROC_PRIMARY) {
        check_all_ports_link_status();
    }

    return 0;
}

static int
init_clock(void)
{
    rte_timer_subsystem_init();
    uint64_t hz = rte_get_timer_hz();
    uint64_t intrs = US_PER_S / lazybsd_global_cfg.freebsd.hz;
    uint64_t tsc = (hz + US_PER_S - 1) / US_PER_S * intrs;

    rte_timer_init(&freebsd_clock);
    rte_timer_reset(&freebsd_clock, tsc, PERIODICAL,
        rte_lcore_id(), &lazybsd_hardclock_job, NULL);

    lazybsd_update_current_ts();

    return 0;
}

#if defined(LAZYBSD_FLOW_ISOLATE) || defined(LAZYBSD_FDIR)
/** Print a message out of a flow error. */
static int
port_flow_complain(struct rte_flow_error *error)
{
    static const char *const errstrlist[] = {
        [RTE_FLOW_ERROR_TYPE_NONE] = "no error",
        [RTE_FLOW_ERROR_TYPE_UNSPECIFIED] = "cause unspecified",
        [RTE_FLOW_ERROR_TYPE_HANDLE] = "flow rule (handle)",
        [RTE_FLOW_ERROR_TYPE_ATTR_GROUP] = "group field",
        [RTE_FLOW_ERROR_TYPE_ATTR_PRIORITY] = "priority field",
        [RTE_FLOW_ERROR_TYPE_ATTR_INGRESS] = "ingress field",
        [RTE_FLOW_ERROR_TYPE_ATTR_EGRESS] = "egress field",
        [RTE_FLOW_ERROR_TYPE_ATTR_TRANSFER] = "transfer field",
        [RTE_FLOW_ERROR_TYPE_ATTR] = "attributes structure",
        [RTE_FLOW_ERROR_TYPE_ITEM_NUM] = "pattern length",
        [RTE_FLOW_ERROR_TYPE_ITEM_SPEC] = "item specification",
        [RTE_FLOW_ERROR_TYPE_ITEM_LAST] = "item specification range",
        [RTE_FLOW_ERROR_TYPE_ITEM_MASK] = "item specification mask",
        [RTE_FLOW_ERROR_TYPE_ITEM] = "specific pattern item",
        [RTE_FLOW_ERROR_TYPE_ACTION_NUM] = "number of actions",
        [RTE_FLOW_ERROR_TYPE_ACTION_CONF] = "action configuration",
        [RTE_FLOW_ERROR_TYPE_ACTION] = "specific action",
    };
    const char *errstr;
    char buf[32];
    int err = rte_errno;

    if ((unsigned int)error->type >= RTE_DIM(errstrlist) ||
        !errstrlist[error->type])
        errstr = "unknown type";
    else
        errstr = errstrlist[error->type];
    printf("Caught error type %d (%s): %s%s: %s\n",
           error->type, errstr,
           error->cause ? (snprintf(buf, sizeof(buf), "cause: %p, ",
                                    error->cause), buf) : "",
           error->message ? error->message : "(no stated reason)",
           rte_strerror(err));
    return -err;
}
#endif


#ifdef LAZYBSD_FLOW_ISOLATE
static int
port_flow_isolate(uint16_t port_id, int set)
{
    struct rte_flow_error error;

    /* Poisoning to make sure PMDs update it in case of error. */
    memset(&error, 0x66, sizeof(error));
    if (rte_flow_isolate(port_id, set, &error))
        return port_flow_complain(&error);
    printf("Ingress traffic on port %u is %s to the defined flow rules\n",
           port_id,
           set ? "now restricted" : "not restricted anymore");
    return 0;
}

static int
create_tcp_flow(uint16_t port_id, uint16_t tcp_port) {
  struct rte_flow_attr attr = {.ingress = 1};
  struct lazybsd_port_cfg *pconf = &lazybsd_global_cfg.dpdk.port_cfgs[port_id];
  int nb_queues = pconf->nb_lcores;
  uint16_t queue[RTE_MAX_QUEUES_PER_PORT];
  int i = 0, j = 0;
  for (i = 0, j = 0; i < nb_queues; ++i)
   queue[j++] = i;
  struct rte_flow_action_rss rss = {
   .types = RTE_ETH_RSS_NONFRAG_IPV4_TCP,
   .key_len = rsskey_len,
   .key = rsskey,
   .queue_num = j,
   .queue = queue,
  };

  struct rte_eth_dev_info dev_info;
  int ret = rte_eth_dev_info_get(port_id, &dev_info);
  if (ret != 0)
    rte_exit(EXIT_FAILURE, "Error during getting device (port %u) info: %s\n", port_id, strerror(-ret));

  struct rte_flow_item pattern[3];
  struct rte_flow_action action[2];
  struct rte_flow_item_tcp tcp_spec;
  struct rte_flow_item_tcp tcp_mask = {
          .hdr = {
                  .src_port = RTE_BE16(0x0000),
                  .dst_port = RTE_BE16(0xffff),
          },
  };
  struct rte_flow_error error;

  memset(pattern, 0, sizeof(pattern));
  memset(action, 0, sizeof(action));

  /* set the dst ipv4 packet to the required value */
  pattern[0].type = RTE_FLOW_ITEM_TYPE_IPV4;

  memset(&tcp_spec, 0, sizeof(struct rte_flow_item_tcp));
  tcp_spec.hdr.dst_port = rte_cpu_to_be_16(tcp_port);
  pattern[1].type = RTE_FLOW_ITEM_TYPE_TCP;
  pattern[1].spec = &tcp_spec;
  pattern[1].mask = &tcp_mask;

  /* end the pattern array */
  pattern[2].type = RTE_FLOW_ITEM_TYPE_END;

  /* create the action */
  action[0].type = RTE_FLOW_ACTION_TYPE_RSS;
  action[0].conf = &rss;
  action[1].type = RTE_FLOW_ACTION_TYPE_END;

  struct rte_flow *flow;
  /* validate and create the flow rule */
  if (!rte_flow_validate(port_id, &attr, pattern, action, &error)) {
      flow = rte_flow_create(port_id, &attr, pattern, action, &error);
      if (!flow) {
          return port_flow_complain(&error);
      }
  }

  memset(pattern, 0, sizeof(pattern));

  /* set the dst ipv4 packet to the required value */
  pattern[0].type = RTE_FLOW_ITEM_TYPE_IPV4;

  struct rte_flow_item_tcp tcp_src_mask = {
          .hdr = {
                  .src_port = RTE_BE16(0xffff),
                  .dst_port = RTE_BE16(0x0000),
          },
  };

  memset(&tcp_spec, 0, sizeof(struct rte_flow_item_tcp));
  tcp_spec.hdr.src_port = rte_cpu_to_be_16(tcp_port);
  pattern[1].type = RTE_FLOW_ITEM_TYPE_TCP;
  pattern[1].spec = &tcp_spec;
  pattern[1].mask = &tcp_src_mask;

  /* end the pattern array */
  pattern[2].type = RTE_FLOW_ITEM_TYPE_END;

  /* validate and create the flow rule */
  if (!rte_flow_validate(port_id, &attr, pattern, action, &error)) {
      flow = rte_flow_create(port_id, &attr, pattern, action, &error);
      if (!flow) {
          return port_flow_complain(&error);
      }
  }

  return 1;
}

static int
init_flow(uint16_t port_id, uint16_t tcp_port) {
  // struct lazybsd_flow_cfg fcfg = lazybsd_global_cfg.dpdk.flow_cfgs[0];

  // int i;
  // for (i = 0; i < fcfg.nb_port; i++) {
  //     if(!create_tcp_flow(fcfg.port_id, fcfg.tcp_ports[i])) {
  //         return 0;
  //     }
  // }

  if(!create_tcp_flow(port_id, tcp_port)) {
      rte_exit(EXIT_FAILURE, "create tcp flow failed\n");
      return -1;
  }

  /*  ARP rule */
  struct rte_flow_attr attr = {.ingress = 1};
  struct rte_flow_action_queue queue = {.index = 0};

  struct rte_flow_item pattern_[2];
  struct rte_flow_action action[2];
  struct rte_flow_item_eth eth_type = {.type = RTE_BE16(0x0806)};
  struct rte_flow_item_eth eth_mask = {
          .type = RTE_BE16(0xffff)
  };

  memset(pattern_, 0, sizeof(pattern_));
  memset(action, 0, sizeof(action));

  pattern_[0].type = RTE_FLOW_ITEM_TYPE_ETH;
  pattern_[0].spec = &eth_type;
  pattern_[0].mask = &eth_mask;

  pattern_[1].type = RTE_FLOW_ITEM_TYPE_END;

  /* create the action */
  action[0].type = RTE_FLOW_ACTION_TYPE_QUEUE;
  action[0].conf = &queue;
  action[1].type = RTE_FLOW_ACTION_TYPE_END;

  struct rte_flow *flow;
  struct rte_flow_error error;
  /* validate and create the flow rule */
  if (!rte_flow_validate(port_id, &attr, pattern_, action, &error)) {
      flow = rte_flow_create(port_id, &attr, pattern_, action, &error);
      if (!flow) {
          return port_flow_complain(&error);
      }
  }

  return 1;
}

#endif

#ifdef LAZYBSD_FDIR
/*
 * Flow director allows the traffic to specific port to be processed on the
 * specific queue. Unlike LAZYBSD_FLOW_ISOLATE, the LAZYBSD_FDIR implementation uses
 * general flow rule so that most FDIR supported NIC will support. The best
 * using case of FDIR is (but not limited to), using multiple processes to
 * listen on different ports.
 *
 * This function can be called either in FSTACK or in end-application.
 *
 * Example:
 *  Given 2 fstack instances A and B. Instance A listens on port 80, and
 *  instance B listens on port 81. We want to process the traffic to port 80
 *  on rx queue 0, and the traffic to port 81 on rx queue 1.
 *  // port 80 rx queue 0
 *  ret = fdir_add_tcp_flow(port_id, 0, LAZYBSD_FLOW_INGRESS, 0, 80);
 *  // port 81 rx queue 1
 *  ret = fdir_add_tcp_flow(port_id, 1, LAZYBSD_FLOW_INGRESS, 0, 81);
 */
#define LAZYBSD_FLOW_EGRESS		1
#define LAZYBSD_FLOW_INGRESS		2
/**
 * Create a flow rule that moves packets with matching src and dest tcp port
 * to the target queue.
 *
 * This function uses general flow rules and doesn't rely on the flow_isolation
 * that not all the FDIR capable NIC support.
 *
 * @param port_id
 *   The selected port.
 * @param queue
 *   The target queue.
 * @param dir
 *   The direction of the traffic.
 *   1 for egress, 2 for ingress and sum(1+2) for both.
 * @param tcp_sport
 *   The src tcp port to match.
 * @param tcp_dport
 *   The dest tcp port to match.
 *
 */
static int
fdir_add_tcp_flow(uint16_t port_id, uint16_t queue, uint16_t dir,
		uint16_t tcp_sport, uint16_t tcp_dport)
{
    struct rte_flow_attr attr;
    struct rte_flow_item flow_pattern[4];
    struct rte_flow_action flow_action[2];
    struct rte_flow *flow = NULL;
    struct rte_flow_action_queue flow_action_queue = { .index = queue };
    struct rte_flow_item_tcp tcp_spec;
    struct rte_flow_item_tcp tcp_mask;
    struct rte_flow_error rfe;
    int res;

    memset(flow_pattern, 0, sizeof(flow_pattern));
    memset(flow_action, 0, sizeof(flow_action));

    /*
     * set the rule attribute.
     */
    memset(&attr, 0, sizeof(struct rte_flow_attr));
    attr.ingress = ((dir & LAZYBSD_FLOW_INGRESS) > 0);
    attr.egress = ((dir & LAZYBSD_FLOW_EGRESS) > 0);

    /*
     * create the action sequence.
     * one action only, move packet to queue
     */
    flow_action[0].type = RTE_FLOW_ACTION_TYPE_QUEUE;
    flow_action[0].conf = &flow_action_queue;
    flow_action[1].type = RTE_FLOW_ACTION_TYPE_END;

    flow_pattern[0].type = RTE_FLOW_ITEM_TYPE_ETH;
    flow_pattern[1].type = RTE_FLOW_ITEM_TYPE_IPV4;

    /*
     * set the third level of the pattern (TCP).
     */
    memset(&tcp_spec, 0, sizeof(struct rte_flow_item_tcp));
    memset(&tcp_mask, 0, sizeof(struct rte_flow_item_tcp));
    tcp_spec.hdr.src_port = htons(tcp_sport);
    tcp_mask.hdr.src_port = (tcp_sport == 0 ? 0: 0xffff);
    tcp_spec.hdr.dst_port = htons(tcp_dport);
    tcp_mask.hdr.dst_port = (tcp_dport == 0 ? 0: 0xffff);
    flow_pattern[2].type = RTE_FLOW_ITEM_TYPE_TCP;
    flow_pattern[2].spec = &tcp_spec;
    flow_pattern[2].mask = &tcp_mask;

    flow_pattern[3].type = RTE_FLOW_ITEM_TYPE_END;

    res = rte_flow_validate(port_id, &attr, flow_pattern, flow_action, &rfe);
    if (res)
	return (1);

    flow = rte_flow_create(port_id, &attr, flow_pattern, flow_action, &rfe);
    if (!flow)
	return port_flow_complain(&rfe);

    return (0);
}

#endif

int
lazybsd_dpdk_init(int argc, char **argv)
{
    if (lazybsd_global_cfg.dpdk.nb_procs < 1 ||
        lazybsd_global_cfg.dpdk.nb_procs > RTE_MAX_LCORE ||
        lazybsd_global_cfg.dpdk.proc_id >= lazybsd_global_cfg.dpdk.nb_procs ||
        lazybsd_global_cfg.dpdk.proc_id < 0) {
        printf("param num_procs[%d] or proc_id[%d] error!\n",
            lazybsd_global_cfg.dpdk.nb_procs,
            lazybsd_global_cfg.dpdk.proc_id);
        exit(1);
    }

    int ret = rte_eal_init(argc, argv);
    if (ret < 0) {
        rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");
    }

    numa_on = lazybsd_global_cfg.dpdk.numa_on;

    idle_sleep = lazybsd_global_cfg.dpdk.idle_sleep;
    pkt_tx_delay = lazybsd_global_cfg.dpdk.pkt_tx_delay > BURST_TX_DRAIN_US ? \
        BURST_TX_DRAIN_US : lazybsd_global_cfg.dpdk.pkt_tx_delay;

    init_lcore_conf();

    init_mem_pool();

    init_dispatch_ring();

    init_msg_ring();

#ifdef LAZYBSD_KNI
    enable_kni = lazybsd_global_cfg.kni.enable;
    if (enable_kni) {
        init_kni();
    }
#endif

#ifdef LAZYBSD_USE_PAGE_ARRAY
    lazybsd_mmap_init();
#endif

#ifdef LAZYBSD_FLOW_ISOLATE
    // run once in primary process
    if (0 == lcore_conf.tx_queue_id[0]){
        ret = port_flow_isolate(0, 1);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "init_port_isolate failed\n");
    }
#endif

    ret = init_port_start();
    if (ret < 0) {
        rte_exit(EXIT_FAILURE, "init_port_start failed\n");
    }

    init_clock();
#ifdef LAZYBSD_FLOW_ISOLATE
    //Only give a example usage: port_id=0, tcp_port= 80.
    //Recommend:
    //1. init_flow should replace `set_rss_table` in `init_port_start` loop, This can set all NIC's port_id_list instead only 0 device(port_id).
    //2. using config options `tcp_port` replace magic number of 80
    ret = init_flow(0, 80);
    if (ret < 0) {
        rte_exit(EXIT_FAILURE, "init_port_flow failed\n");
    }
#endif

#ifdef LAZYBSD_FDIR
    /*
     * Refer function header section for usage.
     */
    ret = fdir_add_tcp_flow(0, 0, LAZYBSD_FLOW_INGRESS, 0, 80);
    if (ret)
	rte_exit(EXIT_FAILURE, "fdir_add_tcp_flow failed\n");
#endif

    return 0;
}

static void
lazybsd_veth_input(const struct lazybsd_dpdk_if_context *ctx, struct rte_mbuf *pkt)
{
    uint8_t rx_csum = ctx->hw_features.rx_csum;
    if (rx_csum) {
        if (pkt->ol_flags & (RTE_MBUF_F_RX_IP_CKSUM_BAD | RTE_MBUF_F_RX_L4_CKSUM_BAD)) {
            rte_pktmbuf_free(pkt);
            return;
        }
    }

    void *data = rte_pktmbuf_mtod(pkt, void*);
    uint16_t len = rte_pktmbuf_data_len(pkt);

    void *hdr = lazybsd_mbuf_gethdr(pkt, pkt->pkt_len, data, len, rx_csum);
    if (hdr == NULL) {
        rte_pktmbuf_free(pkt);
        return;
    }

    if (pkt->ol_flags & RTE_MBUF_F_RX_VLAN_STRIPPED) {
        lazybsd_mbuf_set_vlan_info(hdr, pkt->vlan_tci);
    }

    struct rte_mbuf *pn = pkt->next;
    void *prev = hdr;
    while(pn != NULL) {
        data = rte_pktmbuf_mtod(pn, void*);
        len = rte_pktmbuf_data_len(pn);

        void *mb = lazybsd_mbuf_get(prev, pn, data, len);
        if (mb == NULL) {
            lazybsd_mbuf_free(hdr);
            rte_pktmbuf_free(pkt);
            return;
        }
        pn = pn->next;
        prev = mb;
    }

    lazybsd_veth_process_packet(ctx->ifp, hdr);
}

static enum FilterReturn
protocol_filter(const void *data, uint16_t len)
{
    if(len < RTE_ETHER_ADDR_LEN)
        return FILTER_UNKNOWN;

    const struct rte_ether_hdr *hdr;
    const struct rte_vlan_hdr *vlanhdr;
    hdr = (const struct rte_ether_hdr *)data;
    uint16_t ether_type = rte_be_to_cpu_16(hdr->ether_type);
    data += RTE_ETHER_HDR_LEN;
    len -= RTE_ETHER_HDR_LEN;

    if (ether_type == RTE_ETHER_TYPE_VLAN) {
        vlanhdr = (struct rte_vlan_hdr *)data;
        ether_type = rte_be_to_cpu_16(vlanhdr->eth_proto);
        data += sizeof(struct rte_vlan_hdr);
        len -= sizeof(struct rte_vlan_hdr);
    }

    if(ether_type == RTE_ETHER_TYPE_ARP)
        return FILTER_ARP;

#if (!defined(__FreeBSD__) && defined(INET6) ) || \
    ( defined(__FreeBSD__) && defined(INET6) && defined(LAZYBSD_KNI))
    if (ether_type == RTE_ETHER_TYPE_IPV6) {
        return lazybsd_kni_proto_filter(data,
            len, ether_type);
    }
#endif

#ifndef LAZYBSD_KNI
    return FILTER_UNKNOWN;
#else
    if (!enable_kni) {
        return FILTER_UNKNOWN;
    }

    if(ether_type != RTE_ETHER_TYPE_IPV4)
        return FILTER_UNKNOWN;

    return lazybsd_kni_proto_filter(data,
        len, ether_type);
#endif
}

static inline void
pktmbuf_deep_attach(struct rte_mbuf *mi, const struct rte_mbuf *m)
{
    struct rte_mbuf *md;
    void *src, *dst;

    dst = rte_pktmbuf_mtod(mi, void *);
    src = rte_pktmbuf_mtod(m, void *);

    mi->data_len = m->data_len;
    rte_memcpy(dst, src, m->data_len);

    mi->port = m->port;
    mi->vlan_tci = m->vlan_tci;
    mi->vlan_tci_outer = m->vlan_tci_outer;
    mi->tx_offload = m->tx_offload;
    mi->hash = m->hash;
    mi->ol_flags = m->ol_flags;
    mi->packet_type = m->packet_type;
}

/* copied from rte_pktmbuf_clone */
static inline struct rte_mbuf *
pktmbuf_deep_clone(const struct rte_mbuf *md,
    struct rte_mempool *mp)
{
    struct rte_mbuf *mc, *mi, **prev;
    uint32_t pktlen;
    uint8_t nseg;

    if (unlikely ((mc = rte_pktmbuf_alloc(mp)) == NULL))
        return NULL;

    mi = mc;
    prev = &mi->next;
    pktlen = md->pkt_len;
    nseg = 0;

    do {
        nseg++;
        pktmbuf_deep_attach(mi, md);
        *prev = mi;
        prev = &mi->next;
    } while ((md = md->next) != NULL &&
        (mi = rte_pktmbuf_alloc(mp)) != NULL);

    *prev = NULL;
    mc->nb_segs = nseg;
    mc->pkt_len = pktlen;

    /* Allocation of new indirect segment failed */
    if (unlikely (mi == NULL)) {
        rte_pktmbuf_free(mc);
        return NULL;
    }

    __rte_mbuf_sanity_check(mc, 1);
    return mc;
}

static inline void
lazybsd_add_vlan_tag(struct rte_mbuf * rtem)
{
    void *data = NULL;

    if (rtem->ol_flags & RTE_MBUF_F_RX_VLAN_STRIPPED) {
        data = rte_pktmbuf_prepend(rtem, sizeof(struct rte_vlan_hdr));
        if (data != NULL) {
            memmove(data, data + sizeof(struct rte_vlan_hdr), RTE_ETHER_HDR_LEN);
            struct rte_ether_hdr *etherhdr = (struct rte_ether_hdr *)data;
            struct rte_vlan_hdr *vlanhdr = (struct rte_vlan_hdr *)(data + RTE_ETHER_HDR_LEN);
            vlanhdr->vlan_tci = rte_cpu_to_be_16(rtem->vlan_tci);
            vlanhdr->eth_proto = etherhdr->ether_type;
            etherhdr->ether_type = rte_cpu_to_be_16(RTE_ETHER_TYPE_VLAN);
        }
    }
}

static inline void
process_packets(uint16_t port_id, uint16_t queue_id, struct rte_mbuf **bufs,
    uint16_t count, const struct lazybsd_dpdk_if_context *ctx, int pkts_from_ring)
{
    struct lcore_conf *qconf = &lcore_conf;
    uint16_t nb_queues = qconf->nb_queue_list[port_id];

    uint16_t i;
    for (i = 0; i < count; i++) {
        struct rte_mbuf *rtem = bufs[i];

        if (unlikely( lazybsd_global_cfg.pcap.enable)) {
            if (!pkts_from_ring) {
                lazybsd_dump_packets( lazybsd_global_cfg.pcap.save_path, rtem, lazybsd_global_cfg.pcap.snap_len, lazybsd_global_cfg.pcap.save_len);
            }
        }

        void *data = rte_pktmbuf_mtod(rtem, void*);
        uint16_t len = rte_pktmbuf_data_len(rtem);

        if (!pkts_from_ring) {
            lazybsd_traffic.rx_packets += rtem->nb_segs;
            lazybsd_traffic.rx_bytes += rte_pktmbuf_pkt_len(rtem);
        }

        if (!pkts_from_ring && packet_dispatcher) {
            uint64_t cur_tsc = rte_rdtsc();
            int ret = (*packet_dispatcher)(data, &len, queue_id, nb_queues);
            usr_cb_tsc += rte_rdtsc() - cur_tsc;
            if (ret == LAZYBSD_DISPATCH_RESPONSE) {
                rte_pktmbuf_pkt_len(rtem) = rte_pktmbuf_data_len(rtem) = len;
                /*
                 * We have not support vlan out strip
                 */
                lazybsd_add_vlan_tag(rtem);
                send_single_packet(rtem, port_id);
                continue;
            }

            if (ret == LAZYBSD_DISPATCH_ERROR || ret >= nb_queues) {
                rte_pktmbuf_free(rtem);
                continue;
            }

            if (ret != queue_id) {
                ret = rte_ring_enqueue(dispatch_ring[port_id][ret], rtem);
                if (ret < 0)
                    rte_pktmbuf_free(rtem);

                continue;
            }
        }

        enum FilterReturn filter = protocol_filter(data, len);
#ifdef INET6
        if (filter == FILTER_ARP || filter == FILTER_NDP) {
#else
        if (filter == FILTER_ARP) {
#endif
            struct rte_mempool *mbuf_pool;
            struct rte_mbuf *mbuf_clone;
            if (!pkts_from_ring) {
                uint16_t j;
                for(j = 0; j < nb_queues; ++j) {
                    if(j == queue_id)
                        continue;

                    unsigned socket_id = 0;
                    if (numa_on) {
                        uint16_t lcore_id = qconf->port_cfgs[port_id].lcore_list[j];
                        socket_id = rte_lcore_to_socket_id(lcore_id);
                    }
                    mbuf_pool = pktmbuf_pool[socket_id];
                    mbuf_clone = pktmbuf_deep_clone(rtem, mbuf_pool);
                    if(mbuf_clone) {
                        int ret = rte_ring_enqueue(dispatch_ring[port_id][j],
                            mbuf_clone);
                        if (ret < 0)
                            rte_pktmbuf_free(mbuf_clone);
                    }
                }
            }

#ifdef LAZYBSD_KNI
            if (enable_kni && rte_eal_process_type() == RTE_PROC_PRIMARY) {
                mbuf_pool = pktmbuf_pool[qconf->socket_id];
                mbuf_clone = pktmbuf_deep_clone(rtem, mbuf_pool);
                if(mbuf_clone) {
                    lazybsd_add_vlan_tag(mbuf_clone);
                    lazybsd_kni_enqueue(port_id, mbuf_clone);
                }
            }
#endif
            lazybsd_veth_input(ctx, rtem);
#ifdef LAZYBSD_KNI
        } else if (enable_kni) {
            if (knictl_action == LAZYBSD_KNICTL_ACTION_ALL_TO_KNI){
                lazybsd_add_vlan_tag(rtem);
                lazybsd_kni_enqueue(port_id, rtem);
            } else if (knictl_action == LAZYBSD_KNICTL_ACTION_ALL_TO_FF){
                lazybsd_veth_input(ctx, rtem);
            } else if (knictl_action == LAZYBSD_KNICTL_ACTION_DEFAULT){
                if (enable_kni &&
                        ((filter == FILTER_KNI && kni_accept) ||
                        (filter == FILTER_UNKNOWN && !kni_accept)) ) {
                    lazybsd_add_vlan_tag(rtem);
                    lazybsd_kni_enqueue(port_id, rtem);
                } else {
                    lazybsd_veth_input(ctx, rtem);
                }
            } else {
                lazybsd_veth_input(ctx, rtem);
            }
#endif
        } else {
            lazybsd_veth_input(ctx, rtem);
        }
    }
}

static inline int
process_dispatch_ring(uint16_t port_id, uint16_t queue_id,
    struct rte_mbuf **pkts_burst, const struct lazybsd_dpdk_if_context *ctx)
{
    /* read packet from ring buf and to process */
    uint16_t nb_rb;
    nb_rb = rte_ring_dequeue_burst(dispatch_ring[port_id][queue_id],
        (void **)pkts_burst, MAX_PKT_BURST, NULL);

    if(nb_rb > 0) {
        process_packets(port_id, queue_id, pkts_burst, nb_rb, ctx, 1);
    }

    return nb_rb;
}

static inline void
handle_sysctl_msg(struct lazybsd_msg *msg)
{
    int ret = lazybsd_sysctl(msg->sysctl.name, msg->sysctl.namelen,
        msg->sysctl.oldp, msg->sysctl.oldlenp, msg->sysctl.newp,
        msg->sysctl.newlen);

    if (ret < 0) {
        msg->result = errno;
    } else {
        msg->result = 0;
    }
}

static inline void
handle_ioctl_msg(struct lazybsd_msg *msg)
{
    int fd, ret;
#ifdef INET6
    if (msg->msg_type == LAZYBSD_IOCTL6) {
        fd = lazybsd_socket(AF_INET6, SOCK_DGRAM, 0);
    } else
#endif
        fd = lazybsd_socket(AF_INET, SOCK_DGRAM, 0);

    if (fd < 0) {
        ret = -1;
        goto done;
    }

    ret = lazybsd_ioctl_freebsd(fd, msg->ioctl.cmd, msg->ioctl.data);

    lazybsd_close(fd);

done:
    if (ret < 0) {
        msg->result = errno;
    } else {
        msg->result = 0;
    }
}

static inline void
handle_route_msg(struct lazybsd_msg *msg)
{
    int ret = lazybsd_rtioctl(msg->route.fib, msg->route.data,
        &msg->route.len, msg->route.maxlen);
    if (ret < 0) {
        msg->result = errno;
    } else {
        msg->result = 0;
    }
}

static inline void
handle_top_msg(struct lazybsd_msg *msg)
{
    msg->top = lazybsd_top_status;
    msg->result = 0;
}

#ifdef LAZYBSD_NETGRAPH
static inline void
handle_ngctl_msg(struct lazybsd_msg *msg)
{
    int ret = lazybsd_ngctl(msg->ngctl.cmd, msg->ngctl.data);
    if (ret < 0) {
        msg->result = errno;
    } else {
        msg->result = 0;
        msg->ngctl.ret = ret;
    }
}
#endif

#ifdef LAZYBSD_IPFW
static inline void
handle_ipfw_msg(struct lazybsd_msg *msg)
{
    int fd, ret;
    fd = lazybsd_socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (fd < 0) {
        ret = -1;
        goto done;
    }

    switch (msg->ipfw.cmd) {
        case LAZYBSD_IPFW_GET:
            ret = lazybsd_getsockopt_freebsd(fd, msg->ipfw.level,
                msg->ipfw.optname, msg->ipfw.optval,
                msg->ipfw.optlen);
            break;
        case LAZYBSD_IPFW_SET:
            ret = lazybsd_setsockopt_freebsd(fd, msg->ipfw.level,
                msg->ipfw.optname, msg->ipfw.optval,
                *(msg->ipfw.optlen));
            break;
        default:
            ret = -1;
            errno = ENOTSUP;
            break;
    }

    lazybsd_close(fd);

done:
    if (ret < 0) {
        msg->result = errno;
    } else {
        msg->result = 0;
    }
}
#endif

static inline void
handle_traffic_msg(struct lazybsd_msg *msg)
{
    msg->traffic = lazybsd_traffic;
    msg->result = 0;
}

void lazybsd_get_traffic(void *buffer)
{
    *(struct lazybsd_traffic_args *)buffer = lazybsd_traffic;
}

#ifdef LAZYBSD_KNI
static inline void
handle_knictl_msg(struct lazybsd_msg *msg)
{
    if (msg->knictl.kni_cmd == LAZYBSD_KNICTL_CMD_SET){
        switch (msg->knictl.kni_action){
            case LAZYBSD_KNICTL_ACTION_ALL_TO_FF: knictl_action = LAZYBSD_KNICTL_ACTION_ALL_TO_FF; msg->result = 0; printf("new kni action: alltoff\n"); break;
            case LAZYBSD_KNICTL_ACTION_ALL_TO_KNI: knictl_action = LAZYBSD_KNICTL_ACTION_ALL_TO_KNI; msg->result = 0; printf("new kni action: alltokni\n"); break;
            case LAZYBSD_KNICTL_ACTION_DEFAULT: knictl_action = LAZYBSD_KNICTL_ACTION_DEFAULT; msg->result = 0; printf("new kni action: default\n"); break;
            default: msg->result = -1;
        }
    }
    else if (msg->knictl.kni_cmd == LAZYBSD_KNICTL_CMD_GET){
        msg->knictl.kni_action = knictl_action;
    } else {
        msg->result = -2;
    }
}
#endif

static inline void
handle_default_msg(struct lazybsd_msg *msg)
{
    msg->result = ENOTSUP;
}

static inline void
handle_msg(struct lazybsd_msg *msg, uint16_t proc_id)
{
    switch (msg->msg_type) {
        case LAZYBSD_SYSCTL:
            handle_sysctl_msg(msg);
            break;
        case LAZYBSD_IOCTL:
#ifdef INET6
        case LAZYBSD_IOCTL6:
#endif
            handle_ioctl_msg(msg);
            break;
        case LAZYBSD_ROUTE:
            handle_route_msg(msg);
            break;
        case LAZYBSD_TOP:
            handle_top_msg(msg);
            break;
#ifdef LAZYBSD_NETGRAPH
        case LAZYBSD_NGCTL:
            handle_ngctl_msg(msg);
            break;
#endif
#ifdef LAZYBSD_IPFW
        case LAZYBSD_IPFW_CTL:
            handle_ipfw_msg(msg);
            break;
#endif
        case LAZYBSD_TRAFFIC:
            handle_traffic_msg(msg);
            break;
#ifdef LAZYBSD_KNI
        case LAZYBSD_KNICTL:
            handle_knictl_msg(msg);
            break;
#endif
        default:
            handle_default_msg(msg);
            break;
    }
    if (rte_ring_enqueue(msg_ring[proc_id].ring[msg->msg_type], msg) < 0) {
        if (msg->original_buf) {
            rte_free(msg->buf_addr);
            msg->buf_addr = msg->original_buf;
            msg->buf_len = msg->original_buf_len;
            msg->original_buf = NULL;
        }

        rte_mempool_put(message_pool, msg);
    }
}

static inline int
process_msg_ring(uint16_t proc_id, struct rte_mbuf **pkts_burst)
{
    /* read msg from ring buf and to process */
    uint16_t nb_rb;
    int i;

    nb_rb = rte_ring_dequeue_burst(msg_ring[proc_id].ring[0],
        (void **)pkts_burst, MAX_PKT_BURST, NULL);

    if (likely(nb_rb == 0))
        return 0;

    for (i = 0; i < nb_rb; ++i) {
        handle_msg((struct lazybsd_msg *)pkts_burst[i], proc_id);
    }

    return 0;
}

/* Send burst of packets on an output interface */
static inline int
send_burst(struct lcore_conf *qconf, uint16_t n, uint8_t port)
{
    struct rte_mbuf **m_table;
    int ret;
    uint16_t queueid;

    queueid = qconf->tx_queue_id[port];
    m_table = (struct rte_mbuf **)qconf->tx_mbufs[port].m_table;

    if (unlikely(lazybsd_global_cfg.pcap.enable)) {
        uint16_t i;
        for (i = 0; i < n; i++) {
            lazybsd_dump_packets( lazybsd_global_cfg.pcap.save_path, m_table[i],
               lazybsd_global_cfg.pcap.snap_len, lazybsd_global_cfg.pcap.save_len);
        }
    }

    ret = rte_eth_tx_burst(port, queueid, m_table, n);
    lazybsd_traffic.tx_packets += ret;
    uint16_t i;
    for (i = 0; i < ret; i++) {
        lazybsd_traffic.tx_bytes += rte_pktmbuf_pkt_len(m_table[i]);
#ifdef LAZYBSD_USE_PAGE_ARRAY
        if (qconf->tx_mbufs[port].bsd_m_table[i])
            lazybsd_enq_tx_bsdmbuf(port, qconf->tx_mbufs[port].bsd_m_table[i], m_table[i]->nb_segs);
#endif
    }
    if (unlikely(ret < n)) {
        do {
            rte_pktmbuf_free(m_table[ret]);
#ifdef LAZYBSD_USE_PAGE_ARRAY
            if ( qconf->tx_mbufs[port].bsd_m_table[ret] )
                lazybsd_mbuf_free(qconf->tx_mbufs[port].bsd_m_table[ret]);
#endif
        } while (++ret < n);
    }
    return 0;
}

/* Enqueue a single packet, and send burst if queue is filled */
static inline int
send_single_packet(struct rte_mbuf *m, uint8_t port)
{
    uint16_t len;
    struct lcore_conf *qconf;

    qconf = &lcore_conf;
    len = qconf->tx_mbufs[port].len;
    qconf->tx_mbufs[port].m_table[len] = m;
    len++;

    /* enough pkts to be sent */
    if (unlikely(len == MAX_PKT_BURST)) {
        send_burst(qconf, MAX_PKT_BURST, port);
        len = 0;
    }

    qconf->tx_mbufs[port].len = len;
    return 0;
}

int
lazybsd_dpdk_if_send(struct lazybsd_dpdk_if_context *ctx, void *m,
    int total)
{
#ifdef LAZYBSD_USE_PAGE_ARRAY
    struct lcore_conf *qconf = &lcore_conf;
    int    len = 0;

    len = lazybsd_if_send_onepkt(ctx, m,total);
    if (unlikely(len == MAX_PKT_BURST)) {
        send_burst(qconf, MAX_PKT_BURST, ctx->port_id);
        len = 0;
    }
    qconf->tx_mbufs[ctx->port_id].len = len;
    return 0;
#endif
    struct rte_mempool *mbuf_pool = pktmbuf_pool[lcore_conf.socket_id];
    struct rte_mbuf *head = rte_pktmbuf_alloc(mbuf_pool);
    if (head == NULL) {
        lazybsd_mbuf_free(m);
        return -1;
    }

    head->pkt_len = total;
    head->nb_segs = 0;

    int off = 0;
    struct rte_mbuf *cur = head, *prev = NULL;
    while(total > 0) {
        if (cur == NULL) {
            cur = rte_pktmbuf_alloc(mbuf_pool);
            if (cur == NULL) {
                rte_pktmbuf_free(head);
                lazybsd_mbuf_free(m);
                return -1;
            }
        }

        if (prev != NULL) {
            prev->next = cur;
        }
        head->nb_segs++;

        prev = cur;
        void *data = rte_pktmbuf_mtod(cur, void*);
        int len = total > RTE_MBUF_DEFAULT_DATAROOM ? RTE_MBUF_DEFAULT_DATAROOM : total;
        int ret = lazybsd_mbuf_copydata(m, data, off, len);
        if (ret < 0) {
            rte_pktmbuf_free(head);
            lazybsd_mbuf_free(m);
            return -1;
        }


        cur->data_len = len;
        off += len;
        total -= len;
        cur = NULL;
    }

    struct lazybsd_tx_offload offload = {0};
    lazybsd_mbuf_tx_offload(m, &offload);

    void *data = rte_pktmbuf_mtod(head, void*);

    if (offload.ip_csum) {
        /* ipv6 not supported yet */
        struct rte_ipv4_hdr *iph;
        int iph_len;
        iph = (struct rte_ipv4_hdr *)(data + RTE_ETHER_HDR_LEN);
        iph_len = (iph->version_ihl & 0x0f) << 2;

        head->ol_flags |= RTE_MBUF_F_TX_IP_CKSUM | RTE_MBUF_F_TX_IPV4;
        head->l2_len = RTE_ETHER_HDR_LEN;
        head->l3_len = iph_len;
    }

    if (ctx->hw_features.tx_csum_l4) {
        struct rte_ipv4_hdr *iph;
        int iph_len;
        iph = (struct rte_ipv4_hdr *)(data + RTE_ETHER_HDR_LEN);
        iph_len = (iph->version_ihl & 0x0f) << 2;

        if (offload.tcp_csum) {
            head->ol_flags |= RTE_MBUF_F_TX_TCP_CKSUM;
            head->l2_len = RTE_ETHER_HDR_LEN;
            head->l3_len = iph_len;
        }

        /*
         *  TCP segmentation offload.
         *
         *  - set the PKT_TX_TCP_SEG flag in mbuf->ol_flags (this flag
         *    implies PKT_TX_TCP_CKSUM)
         *  - set the flag PKT_TX_IPV4 or PKT_TX_IPV6
         *  - if it's IPv4, set the PKT_TX_IP_CKSUM flag and
         *    write the IP checksum to 0 in the packet
         *  - fill the mbuf offload information: l2_len,
         *    l3_len, l4_len, tso_segsz
         *  - calculate the pseudo header checksum without taking ip_len
         *    in account, and set it in the TCP header. Refer to
         *    rte_ipv4_phdr_cksum() and rte_ipv6_phdr_cksum() that can be
         *    used as helpers.
         */
        if (offload.tso_seg_size) {
            struct rte_tcp_hdr *tcph;
            int tcph_len;
            tcph = (struct rte_tcp_hdr *)((char *)iph + iph_len);
            tcph_len = (tcph->data_off & 0xf0) >> 2;
            tcph->cksum = rte_ipv4_phdr_cksum(iph, RTE_MBUF_F_TX_TCP_SEG);

            head->ol_flags |= RTE_MBUF_F_TX_TCP_SEG;
            head->l4_len = tcph_len;
            head->tso_segsz = offload.tso_seg_size;
        }

        if (offload.udp_csum) {
            head->ol_flags |= RTE_MBUF_F_TX_UDP_CKSUM;
            head->l2_len = RTE_ETHER_HDR_LEN;
            head->l3_len = iph_len;
        }
    }

    lazybsd_mbuf_free(m);

    return send_single_packet(head, ctx->port_id);
}

static int
main_loop(void *arg)
{
    struct loop_routine *lr = (struct loop_routine *)arg;

    struct rte_mbuf *pkts_burst[MAX_PKT_BURST];
    uint64_t prev_tsc, dilazybsd_tsc, cur_tsc, usch_tsc, div_tsc, usr_tsc, sys_tsc, end_tsc, idle_sleep_tsc;
    int i, j, nb_rx, idle;
    uint16_t port_id, queue_id;
    struct lcore_conf *qconf;
    uint64_t drain_tsc = 0;
    struct lazybsd_dpdk_if_context *ctx;

    if (pkt_tx_delay) {
        drain_tsc = (rte_get_tsc_hz() + US_PER_S - 1) / US_PER_S * pkt_tx_delay;
    }

    prev_tsc = 0;
    usch_tsc = 0;

    qconf = &lcore_conf;

    while (1) {
        cur_tsc = rte_rdtsc();
        if (unlikely(freebsd_clock.expire < cur_tsc)) {
            rte_timer_manage();
        }

        idle = 1;
        sys_tsc = 0;
        usr_tsc = 0;
        usr_cb_tsc = 0;

        /*
         * TX burst queue drain
         */
        dilazybsd_tsc = cur_tsc - prev_tsc;
        if (unlikely(dilazybsd_tsc >= drain_tsc)) {
            for (i = 0; i < qconf->nb_tx_port; i++) {
                port_id = qconf->tx_port_id[i];
                if (qconf->tx_mbufs[port_id].len == 0)
                    continue;

                idle = 0;

                send_burst(qconf,
                    qconf->tx_mbufs[port_id].len,
                    port_id);
                qconf->tx_mbufs[port_id].len = 0;
            }

            prev_tsc = cur_tsc;
        }

        /*
         * Read packet from RX queues
         */
        for (i = 0; i < qconf->nb_rx_queue; ++i) {
            port_id = qconf->rx_queue_list[i].port_id;
            queue_id = qconf->rx_queue_list[i].queue_id;
            ctx = veth_ctx[port_id];

#ifdef LAZYBSD_KNI
            if (enable_kni && rte_eal_process_type() == RTE_PROC_PRIMARY) {
                lazybsd_kni_process(port_id, queue_id, pkts_burst, MAX_PKT_BURST);
            }
#endif

            idle &= !process_dispatch_ring(port_id, queue_id, pkts_burst, ctx);

            nb_rx = rte_eth_rx_burst(port_id, queue_id, pkts_burst,
                MAX_PKT_BURST);
            if (nb_rx == 0)
                continue;

            idle = 0;

            /* Prefetch first packets */
            for (j = 0; j < PREFETCH_OFFSET && j < nb_rx; j++) {
                rte_prefetch0(rte_pktmbuf_mtod(
                        pkts_burst[j], void *));
            }

            /* Prefetch and handle already prefetched packets */
            for (j = 0; j < (nb_rx - PREFETCH_OFFSET); j++) {
                rte_prefetch0(rte_pktmbuf_mtod(pkts_burst[
                        j + PREFETCH_OFFSET], void *));
                process_packets(port_id, queue_id, &pkts_burst[j], 1, ctx, 0);
            }

            /* Handle remaining prefetched packets */
            for (; j < nb_rx; j++) {
                process_packets(port_id, queue_id, &pkts_burst[j], 1, ctx, 0);
            }
        }

        process_msg_ring(qconf->proc_id, pkts_burst);

        div_tsc = rte_rdtsc();

        if (likely(lr->loop != NULL && (!idle || cur_tsc - usch_tsc >= drain_tsc))) {
            usch_tsc = cur_tsc;
            lr->loop(lr->arg);
        }

        idle_sleep_tsc = rte_rdtsc();
        if (likely(idle && idle_sleep)) {
            usleep(idle_sleep);
            end_tsc = rte_rdtsc();
        } else {
            end_tsc = idle_sleep_tsc;
        }

        usr_tsc = usr_cb_tsc;
        if (usch_tsc == cur_tsc) {
            usr_tsc += idle_sleep_tsc - div_tsc;
        }

        if (!idle) {
            sys_tsc = div_tsc - cur_tsc - usr_cb_tsc;
            lazybsd_top_status.sys_tsc += sys_tsc;
        }

        lazybsd_top_status.usr_tsc += usr_tsc;
        lazybsd_top_status.work_tsc += end_tsc - cur_tsc;
        lazybsd_top_status.idle_tsc += end_tsc - cur_tsc - usr_tsc - sys_tsc;

        lazybsd_top_status.loops++;
    }

    return 0;
}

int
lazybsd_dpdk_if_up(void) {
    int i;
    struct lcore_conf *qconf = &lcore_conf;
    for (i = 0; i < qconf->nb_tx_port; i++) {
        uint16_t port_id = qconf->tx_port_id[i];

        struct lazybsd_port_cfg *pconf = &qconf->port_cfgs[port_id];
        veth_ctx[port_id] = (lazybsd_dpdk_if_context*)lazybsd_veth_attach(pconf);
        if (veth_ctx[port_id] == NULL) {
            rte_exit(EXIT_FAILURE, "lazybsd_veth_attach failed");
        }
    }

    return 0;
}

void
lazybsd_dpdk_run(loop_func_t loop, void *arg) {
    struct loop_routine *lr = (struct loop_routine *)rte_malloc(NULL,
        sizeof(struct loop_routine), 0);
    lr->loop = loop;
    lr->arg = arg;
    rte_eal_mp_remote_launch(main_loop, lr, CALL_MAIN);
    rte_eal_mp_wait_lcore();
    rte_free(lr);
}

void
lazybsd_dpdk_pktmbuf_free(void *m)
{
    rte_pktmbuf_free_seg((struct rte_mbuf *)m);
}

static uint32_t
toeplitz_hash(unsigned keylen, const uint8_t *key,
    unsigned datalen, const uint8_t *data)
{
    uint32_t hash = 0, v;
    u_int i, b;

    /* XXXRW: Perhaps an assertion about key length vs. data length? */

    v = (key[0]<<24) + (key[1]<<16) + (key[2] <<8) + key[3];
    for (i = 0; i < datalen; i++) {
        for (b = 0; b < 8; b++) {
            if (data[i] & (1<<(7-b)))
                hash ^= v;
            v <<= 1;
            if ((i + 4) < keylen &&
                (key[i+4] & (1<<(7-b))))
                v |= 1;
        }
    }
    return (hash);
}

int
lazybsd_in_pcbladdr(uint16_t family, void *faddr, uint16_t fport, void *laddr)
{
    int ret = 0;
    uint16_t fa;

    if (!pcblddr_fun)
        return ret;

    if (family == AF_INET)
        fa = AF_INET;
    else if (family == AF_INET6_FREEBSD)
        fa = AF_INET6_LINUX;
    else
        return EADDRNOTAVAIL;

    ret = (*pcblddr_fun)(fa, faddr, fport, laddr);

    return ret;
}

void
lazybsd_regist_pcblddr_fun(pcblddr_func_t func)
{
    pcblddr_fun = func;
}

int
lazybsd_rss_check(void *softc, uint32_t saddr, uint32_t daddr,
    uint16_t sport, uint16_t dport)
{
    struct lcore_conf *qconf = &lcore_conf;
    struct lazybsd_dpdk_if_context *ctx = (struct lazybsd_dpdk_if_context *)lazybsd_veth_softc_to_hostc(softc);
    uint16_t nb_queues = qconf->nb_queue_list[ctx->port_id];

    if (nb_queues <= 1) {
        return 1;
    }

    uint16_t reta_size = rss_reta_size[ctx->port_id];
    uint16_t queueid = qconf->tx_queue_id[ctx->port_id];

    uint8_t data[sizeof(saddr) + sizeof(daddr) + sizeof(sport) +
        sizeof(dport)];

    unsigned datalen = 0;

    bcopy(&saddr, &data[datalen], sizeof(saddr));
    datalen += sizeof(saddr);

    bcopy(&daddr, &data[datalen], sizeof(daddr));
    datalen += sizeof(daddr);

    bcopy(&sport, &data[datalen], sizeof(sport));
    datalen += sizeof(sport);

    bcopy(&dport, &data[datalen], sizeof(dport));
    datalen += sizeof(dport);

    uint32_t hash = 0;
    hash = toeplitz_hash(rsskey_len, rsskey, datalen, data);

    return ((hash & (reta_size - 1)) % nb_queues) == queueid;
}

void
lazybsd_regist_packet_dispatcher(dispatch_func_t func)
{
    packet_dispatcher = func;
}

uint64_t
lazybsd_get_tsc_ns()
{
    uint64_t cur_tsc = rte_rdtsc();
    uint64_t hz = rte_get_tsc_hz();
    return ((double)cur_tsc/(double)hz) * NS_PER_S;
}

static void
set_bitmap(uint16_t port, unsigned char *bitmap)
{
    port = htons(port);
    unsigned char *p = bitmap + port/8;
    *p = set_bit(*p, port % 8);
}

static int
get_bitmap(uint16_t port, unsigned char *bitmap)
{
    unsigned char *p = bitmap + port/8;
    return get_bit(*p, port % 8) > 0 ? 1 : 0;
}

static void
kni_set_bitmap(const char *p, unsigned char *port_bitmap)
{
    int i;
    const char *head, *tail, *tail_num;
    if(!p)
        return;

    head = p;
    while (1) {
        tail = strstr(head, ",");
        tail_num = strstr(head, "-");
        if(tail_num && (!tail || tail_num < tail - 1)) {
            for(i = atoi(head); i <= atoi(tail_num + 1); ++i) {
                set_bitmap(i, port_bitmap);
            }
        } else {
            set_bitmap(atoi(head), port_bitmap);
        }

        if(!tail)
            break;

        head = tail + 1;
    }
}

#ifdef LAZYBSD_KNI_KNI
/* Currently we don't support change mtu. */
static int
kni_change_mtu(uint16_t port_id, unsigned new_mtu)
{
    return 0;
}

static int
kni_config_network_interface(uint16_t port_id, uint8_t if_up)
{
    int ret = 0;

    if (!rte_eth_dev_is_valid_port(port_id)) {
        printf("Invalid port id %d\n", port_id);
        return -EINVAL;
    }

    printf("Configure network interface of %d %s\n",
            port_id, if_up ? "up" : "down");

    ret = (if_up) ?
        rte_eth_dev_set_link_up(port_id) :
        rte_eth_dev_set_link_down(port_id);

    /*
     * Some NIC drivers will crash in secondary process after config kni , Such as ENA with DPDK-21.22.3.
     * If you meet this crash, you can try disable the code below and return 0 directly.
     * Or run primary first, then config kni interface in kernel, and run secondary processes last.
     */
    if(-ENOTSUP == ret) {
        if (if_up != 0) {
            /* Configure network interface up */
            rte_eth_dev_stop(port_id);
            ret = rte_eth_dev_start(port_id);
        } else {
            /* Configure network interface down */
            rte_eth_dev_stop(port_id);
            ret = 0;
        }
    }

    if (ret < 0)
        printf("Failed to Configure network interface of %d %s\n",
            port_id, if_up ? "up" : "down");

    return ret;
}

static void
print_ethaddr(const char *name, struct rte_ether_addr *mac_addr)
{
    char buf[RTE_ETHER_ADDR_FMT_SIZE];
    rte_ether_format_addr(buf, RTE_ETHER_ADDR_FMT_SIZE, mac_addr);
    printf("\t%s%s\n", name, buf);
}


/* Callback for request of configuring mac address */
static int
kni_config_mac_address(uint16_t port_id, uint8_t mac_addr[])
{
    int ret = 0;

    if (!rte_eth_dev_is_valid_port(port_id)) {
        printf("Invalid port id %d\n", port_id);
        return -EINVAL;
    }

    print_ethaddr("Address:", (struct rte_ether_addr *)mac_addr);

    ret = rte_eth_dev_default_mac_addr_set(port_id,
                       (struct rte_ether_addr *)mac_addr);
    if (ret < 0)
        printf("Failed to config mac_addr for port %d\n", port_id);

    return ret;
}
#endif

static int
kni_process_tx(uint16_t port_id, uint16_t queue_id,
    struct rte_mbuf **pkts_burst, unsigned count)
{
    /* read packet from kni ring(phy port) and transmit to kni */
    uint16_t nb_tx, nb_kni_tx = 0;
    nb_tx = rte_ring_dequeue_burst(kni_rp[port_id], (void **)pkts_burst, count, NULL);

#ifdef LAZYBSD_KNI_KNI
    if (lazybsd_global_cfg.kni.type == KNI_TYPE_KNI) {
        /* NB.
         * if nb_tx is 0,it must call rte_kni_tx_burst
         * must Call regularly rte_kni_tx_burst(kni, NULL, 0).
         * detail https://embedded.communities.intel.com/thread/6668
         */
        nb_kni_tx = rte_kni_tx_burst(kni_stat[port_id]->kni, pkts_burst, nb_tx);
        rte_kni_handle_request(kni_stat[port_id]->kni);
    } else if (lazybsd_global_cfg.kni.type == KNI_TYPE_VIRTIO)
#endif
    {
        nb_kni_tx = rte_eth_tx_burst(kni_stat[port_id]->port_id, 0, pkts_burst, nb_tx);
    }

    if(nb_kni_tx < nb_tx) {
        uint16_t i;
        for(i = nb_kni_tx; i < nb_tx; ++i)
            rte_pktmbuf_free(pkts_burst[i]);

        kni_stat[port_id]->rx_dropped += (nb_tx - nb_kni_tx);
    }

    kni_stat[port_id]->rx_packets += nb_kni_tx;
    return 0;
}

static int
kni_process_rx(uint16_t port_id, uint16_t queue_id,
    struct rte_mbuf **pkts_burst, unsigned count)
{
    uint16_t nb_kni_rx = 0, nb_rx;

#ifdef LAZYBSD_KNI_KNI
    if (lazybsd_global_cfg.kni.type == KNI_TYPE_KNI) {
        /* read packet from kni, and transmit to phy port */
        nb_kni_rx = rte_kni_rx_burst(kni_stat[port_id]->kni, pkts_burst, count);
    } else if (lazybsd_global_cfg.kni.type == KNI_TYPE_VIRTIO)
#endif
    {
        nb_kni_rx = rte_eth_rx_burst(kni_stat[port_id]->port_id, 0, pkts_burst, count);
    }

    if (nb_kni_rx > 0) {
        nb_rx = rte_eth_tx_burst(port_id, queue_id, pkts_burst, nb_kni_rx);
        if (nb_rx < nb_kni_rx) {
            uint16_t i;
            for(i = nb_rx; i < nb_kni_rx; ++i)
                rte_pktmbuf_free(pkts_burst[i]);

            kni_stat[port_id]->tx_dropped += (nb_kni_rx - nb_rx);
        }

        kni_stat[port_id]->tx_packets += nb_rx;
    }
    return 0;
}

static enum FilterReturn
protocol_filter_l4(uint16_t port, unsigned char *bitmap)
{
    if(get_bitmap(port, bitmap)) {
        return FILTER_KNI;
    }

    return FILTER_UNKNOWN;
}

static enum FilterReturn
protocol_filter_tcp(const void *data, uint16_t len)
{
    if (len < sizeof(struct rte_tcp_hdr))
        return FILTER_UNKNOWN;

    const struct rte_tcp_hdr *hdr;
    hdr = (const struct rte_tcp_hdr *)data;

    return protocol_filter_l4(hdr->dst_port, tcp_port_bitmap);
}

static enum FilterReturn
protocol_filter_udp(const void* data,uint16_t len)
{
    if (len < sizeof(struct rte_udp_hdr))
        return FILTER_UNKNOWN;

    const struct rte_udp_hdr *hdr;
    hdr = (const struct rte_udp_hdr *)data;

    return protocol_filter_l4(hdr->dst_port, udp_port_bitmap);
}

#ifdef INET6
/*
 * https://www.iana.org/assignments/ipv6-parameters/ipv6-parameters.xhtml
 */
#ifndef IPPROTO_HIP
#define IPPROTO_HIP 139
#endif

#ifndef IPPROTO_SHIM6
#define IPPROTO_SHIM6   140
#endif

#ifndef IPPROTO_MH
#define IPPROTO_MH   135
#endif
static int
get_ipv6_hdr_len(uint8_t *proto, void *data, uint16_t len)
{
    int ext_hdr_len = 0;

    switch (*proto) {
        case IPPROTO_HOPOPTS:   case IPPROTO_ROUTING:   case IPPROTO_DSTOPTS:
        case IPPROTO_MH:        case IPPROTO_HIP:       case IPPROTO_SHIM6:
            ext_hdr_len = *((uint8_t *)data + 1) + 1;
            break;
        case IPPROTO_FRAGMENT:
            ext_hdr_len = 8;
            break;
        case IPPROTO_AH:
            ext_hdr_len = (*((uint8_t *)data + 1) + 2) * 4;
            break;
        case IPPROTO_NONE:
#ifdef LAZYBSD_IPSEC
        case IPPROTO_ESP:
            //proto = *((uint8_t *)data + len - 1 - 4);
            //ext_hdr_len = len;
#endif
        default:
            return ext_hdr_len;
    }

    if (ext_hdr_len >= len) {
        return len;
    }

    *proto = *((uint8_t *)data);
    ext_hdr_len += get_ipv6_hdr_len(proto, data + ext_hdr_len, len - ext_hdr_len);

    return ext_hdr_len;
}

static enum FilterReturn
protocol_filter_icmp6(void *data, uint16_t len)
{
    if (len < sizeof(struct icmp6_hdr))
        return FILTER_UNKNOWN;

    const struct icmp6_hdr *hdr;
    hdr = (const struct icmp6_hdr *)data;

    if (hdr->icmp6_type >= ND_ROUTER_SOLICIT && hdr->icmp6_type <= ND_REDIRECT)
        return FILTER_NDP;

    return FILTER_UNKNOWN;
}
#endif

static enum FilterReturn
protocol_filter_ip(const void *data, uint16_t len, uint16_t eth_frame_type)
{
    uint8_t proto;
    int hdr_len;
    void *next;
    uint16_t next_len;

    if (eth_frame_type == RTE_ETHER_TYPE_IPV4) {
        if(len < sizeof(struct rte_ipv4_hdr))
            return FILTER_UNKNOWN;

        const struct rte_ipv4_hdr *hdr = (struct rte_ipv4_hdr *)data;
        hdr_len = (hdr->version_ihl & 0x0f) << 2;
        if (len < hdr_len)
            return FILTER_UNKNOWN;

        proto = hdr->next_proto_id;
#ifdef INET6
    } else if(eth_frame_type == RTE_ETHER_TYPE_IPV6) {
        if(len < sizeof(struct rte_ipv6_hdr))
            return FILTER_UNKNOWN;

        hdr_len = sizeof(struct rte_ipv6_hdr);
        proto = ((struct rte_ipv6_hdr *)data)->proto;
        hdr_len += get_ipv6_hdr_len(&proto, (void *)data + hdr_len, len - hdr_len);

        if (len < hdr_len)
            return FILTER_UNKNOWN;
#endif
    } else {
        return FILTER_UNKNOWN;
    }

    next = (void *)data + hdr_len;
    next_len = len - hdr_len;

    switch (proto) {
        case IPPROTO_TCP:
#ifdef LAZYBSD_KNI
            if (!enable_kni)
                break;
#else
            break;
#endif
            return protocol_filter_tcp(next, next_len);
        case IPPROTO_UDP:
#ifdef LAZYBSD_KNI
            if (!enable_kni)
                break;
#else
            break;
#endif
            return protocol_filter_udp(next, next_len);
        case IPPROTO_IPIP:
            return protocol_filter_ip(next, next_len, RTE_ETHER_TYPE_IPV4);
#ifdef INET6
        case IPPROTO_IPV6:
            return protocol_filter_ip(next, next_len, RTE_ETHER_TYPE_IPV6);
        case IPPROTO_ICMPV6:
            return protocol_filter_icmp6(next, next_len);
#endif
    }

    return FILTER_UNKNOWN;
}

enum FilterReturn
lazybsd_kni_proto_filter(const void *data, uint16_t len, uint16_t eth_frame_type)
{
    return protocol_filter_ip(data, len, eth_frame_type);
}

void
lazybsd_kni_init(uint16_t nb_ports, int type, const char *tcp_ports, const char *udp_ports)
{
    if (rte_eal_process_type() == RTE_PROC_PRIMARY) {
        kni_stat = (struct kni_interface_stats **)rte_zmalloc("kni:stat",
            sizeof(struct kni_interface_stats *) * nb_ports,
            RTE_CACHE_LINE_SIZE);
        if (kni_stat == NULL)
            rte_exit(EXIT_FAILURE, "rte_zmalloc(1 (struct netio_kni_stat *)) "
                "failed\n");

        if (type == KNI_TYPE_KNI) {
#ifdef LAZYBSD_KNI_KNI
            rte_kni_init(nb_ports);
#endif
        }
    }

    uint16_t lcoreid = rte_lcore_id();
    char name_buf[RTE_RING_NAMESIZE];
    snprintf(name_buf, RTE_RING_NAMESIZE, "kni::ring_%d", lcoreid);
    kni_rp = (struct rte_ring **)rte_zmalloc(name_buf,
            sizeof(struct rte_ring *) * nb_ports,
            RTE_CACHE_LINE_SIZE);
    if (kni_rp == NULL) {
        rte_exit(EXIT_FAILURE, "rte_zmalloc(%s (struct rte_ring*)) "
                "failed\n", name_buf);
    }

    snprintf(name_buf, RTE_RING_NAMESIZE, "kni:tcp_port_bitmap_%d", lcoreid);
    tcp_port_bitmap = (unsigned char *)rte_zmalloc("kni:tcp_port_bitmap", 8192,
        RTE_CACHE_LINE_SIZE);
    if (tcp_port_bitmap == NULL) {
        rte_exit(EXIT_FAILURE, "rte_zmalloc(%s (tcp_port_bitmap)) "
                "failed\n", name_buf);
    }

    snprintf(name_buf, RTE_RING_NAMESIZE, "kni:udp_port_bitmap_%d", lcoreid);
    udp_port_bitmap = (unsigned char *)rte_zmalloc("kni:udp_port_bitmap", 8192,
        RTE_CACHE_LINE_SIZE);
    if (udp_port_bitmap == NULL) {
        rte_exit(EXIT_FAILURE, "rte_zmalloc(%s (udp_port_bitmap)) "
                "failed\n",name_buf);
    }

    memset(tcp_port_bitmap, 0, 8192);
    memset(udp_port_bitmap, 0, 8192);

    kni_set_bitmap(tcp_ports, tcp_port_bitmap);
    kni_set_bitmap(udp_ports, udp_port_bitmap);
}

void
lazybsd_kni_alloc(uint16_t port_id, unsigned socket_id, int type, int port_idx,
    struct rte_mempool *mbuf_pool, unsigned ring_queue_size)
{
    if (rte_eal_process_type() == RTE_PROC_PRIMARY) {
        struct rte_eth_dev_info dev_info;
        struct rte_ether_addr addr = {0};
        int ret;

        kni_stat[port_id] = (struct kni_interface_stats*)rte_zmalloc(
            "kni:stat_lcore",
            sizeof(struct kni_interface_stats),
            RTE_CACHE_LINE_SIZE);

        if (kni_stat[port_id] == NULL) {
            rte_panic("rte_zmalloc kni_interface_stats failed\n");
        }

        kni_stat[port_id]->rx_packets = 0;
        kni_stat[port_id]->rx_dropped = 0;
        kni_stat[port_id]->tx_packets = 0;
        kni_stat[port_id]->tx_dropped = 0;

        memset(&dev_info, 0, sizeof(dev_info));
        ret = rte_eth_dev_info_get(port_id, &dev_info);
        if (ret != 0) {
            rte_panic("kni get dev info %u failed!\n", port_id);
        }

        /* Get the interface default mac address */
        rte_eth_macaddr_get(port_id,
                (struct rte_ether_addr *)&addr);

        printf("lazybsd_kni_alloc get Port %u MAC:"RTE_ETHER_ADDR_PRT_FMT"\n",
            (unsigned)port_id, RTE_ETHER_ADDR_BYTES(&addr));

#ifdef LAZYBSD_KNI_KNI
        if (type == KNI_TYPE_KNI) {
            struct rte_kni_conf conf;
            struct rte_kni_ops ops;

            /* only support one kni */
            memset(&conf, 0, sizeof(conf));
            snprintf(conf.name, RTE_KNI_NAMESIZE, "veth%u", port_id);
            conf.core_id = rte_lcore_id();
            conf.force_bind = 1;
            conf.group_id = port_id;
            uint16_t mtu;
            rte_eth_dev_get_mtu(port_id, &mtu);
            conf.mbuf_size = mtu + KNI_ENET_HEADER_SIZE + KNI_ENET_FCS_SIZE;
            rte_memcpy(&conf.addr, addr.addr_bytes, RTE_ETHER_ADDR_LEN);

            memset(&ops, 0, sizeof(ops));
            ops.port_id = port_id;
            ops.change_mtu = kni_change_mtu;
            ops.config_network_if = kni_config_network_interface;
            ops.config_mac_address = kni_config_mac_address;

            kni_stat[port_id]->kni = rte_kni_alloc(mbuf_pool, &conf, &ops);
            if (kni_stat[port_id]->kni == NULL)
                rte_panic("create kni on port %u failed!\n", port_id);
            else
                printf("create kni on port %u success!\n", port_id);

            kni_stat[port_id]->port_id = port_id;
        } else if (type == KNI_TYPE_VIRTIO)
#endif
        {
            /*
             * to add virtio port for exception path(KNI),
             * see https://doc.dpdk.org/guides/howto/virtio_user_as_exception_path.html#virtio-user-as-exception-path
             */
            char port_name[32];
            char port_args[256];

            /* set the name and arguments */
            snprintf(port_name, sizeof(port_name), "virtio_user%u", port_id);
            snprintf(port_args, sizeof(port_args),
                "path=/dev/vhost-net,queues=1,queue_size=%u,iface=veth%d,mac=" RTE_ETHER_ADDR_PRT_FMT,
                ring_queue_size, port_id, RTE_ETHER_ADDR_BYTES(&addr));
            printf("lazybsd_kni_alloc to rte_eal_hotplug_add virtio user port, portname:%s, portargs:%s\n",
                port_name, port_args);

            /* add the vdev for virtio_user */
            if (rte_eal_hotplug_add("vdev", port_name, port_args) < 0) {
                rte_exit(EXIT_FAILURE, "lazybsd_kni_alloc cannot create virtio user paired port for port %u\n", port_id);
            }

            kni_stat[port_id]->port_id = port_idx + nb_dev_ports;
        }
    }

    char ring_name[RTE_KNI_NAMESIZE];
    snprintf((char*)ring_name, RTE_KNI_NAMESIZE, "kni_ring_%u", port_id);

    if (rte_eal_process_type() == RTE_PROC_PRIMARY) {
        kni_rp[port_id] = rte_ring_create(ring_name, ring_queue_size,
            socket_id, RING_F_SC_DEQ);

        if (rte_ring_lookup(ring_name) != kni_rp[port_id])
            rte_panic("lookup kni ring failed!\n");
    } else {
        kni_rp[port_id] = rte_ring_lookup(ring_name);
    }

    if (kni_rp[port_id] == NULL)
        rte_panic("create kni ring failed!\n");

    printf("create kni ring success, %u ring entries are now free!\n",
        rte_ring_free_count(kni_rp[port_id]));
}

void
lazybsd_kni_process(uint16_t port_id, uint16_t queue_id,
    struct rte_mbuf **pkts_burst, unsigned count)
{
    kni_process_tx(port_id, queue_id, pkts_burst, count);
    kni_process_rx(port_id, queue_id, pkts_burst, count);
}

/* enqueue the packet, and own it */
int
lazybsd_kni_enqueue(uint16_t port_id, struct rte_mbuf *pkt)
{
    int ret = rte_ring_enqueue(kni_rp[port_id], pkt);
    if (ret < 0)
        rte_pktmbuf_free(pkt);

    return 0;
}


int lazybsd_enable_pcap(const char* dump_path, uint16_t snap_len)
{
    char pcap_f_path[FILE_PATH_LEN] = {0};

    snprintf(pcap_f_path, FILE_PATH_LEN,  "%s/cpu%d_%d.pcap", dump_path==NULL?".":dump_path, rte_lcore_id(), seq);
    g_pcap_fp = fopen(pcap_f_path, "w+");
    if (g_pcap_fp == NULL) { 
        rte_exit(EXIT_FAILURE, "Cannot open pcap dump path: %s, errno %d.\n", pcap_f_path, errno);
        return -1;
    }
    g_flen = 0;

    struct pcap_file_header pcap_file_hdr;
    void* file_hdr = &pcap_file_hdr;

    pcap_file_hdr.magic = 0xA1B2C3D4;
    pcap_file_hdr.version_major = 0x0002;
    pcap_file_hdr.version_minor = 0x0004;
    pcap_file_hdr.thiszone = 0x00000000;
    pcap_file_hdr.sigfigs = 0x00000000;
    pcap_file_hdr.snaplen = snap_len;   //0x0000FFFF;  //65535
    pcap_file_hdr.linktype = 0x00000001; //DLT_EN10MB, Ethernet (10Mb)

    fwrite(file_hdr, sizeof(struct pcap_file_header), 1, g_pcap_fp);
    g_flen += sizeof(struct pcap_file_header);

    return 0;
}

int
lazybsd_dump_packets(const char* dump_path, struct rte_mbuf* pkt, uint16_t snap_len, uint32_t f_maxlen)
{
    unsigned int out_len = 0, wr_len = 0;
    struct pcap_pkthdr pcap_hdr;
    void* hdr = &pcap_hdr;
    struct timeval ts;
    char pcap_f_path[FILE_PATH_LEN] = {0};

    if (g_pcap_fp == NULL) {
        return -1;
    }
    snap_len = pkt->pkt_len < snap_len ? pkt->pkt_len : snap_len;
    gettimeofday(&ts, NULL);
    pcap_hdr.sec = ts.tv_sec;
    pcap_hdr.usec = ts.tv_usec;
    pcap_hdr.caplen = snap_len;
    pcap_hdr.len = pkt->pkt_len;
    fwrite(hdr, sizeof(struct pcap_pkthdr), 1, g_pcap_fp);
    g_flen += sizeof(struct pcap_pkthdr);

    while(pkt != NULL && out_len <= snap_len) {
        wr_len = snap_len - out_len;
        wr_len = wr_len > pkt->data_len ? pkt->data_len : wr_len ;
        fwrite(rte_pktmbuf_mtod(pkt, char*), wr_len, 1, g_pcap_fp);
        out_len += wr_len;
        pkt = pkt->next;
    }
    g_flen += out_len;

    if ( g_flen >= f_maxlen ){
        fclose(g_pcap_fp);
        if ( ++seq >= PCAP_FILE_NUM )
            seq = 0;
        
        lazybsd_enable_pcap(dump_path, snap_len);
    }

    return 0;
}

/******************* Net device related constatns *****************************/
static constexpr uint16_t default_ring_size      = 512;

// 
// We need 2 times the ring size of buffers because of the way PMDs 
// refill the ring.
//
static constexpr uint16_t mbufs_per_queue_rx     = 2 * default_ring_size;
static constexpr uint16_t rx_gc_thresh           = 64;

//
// No need to keep more descriptors in the air than can be sent in a single
// rte_eth_tx_burst() call.
//
static constexpr uint16_t mbufs_per_queue_tx     = 2 * default_ring_size;

static constexpr uint16_t mbuf_cache_size        = 512;
static constexpr uint16_t mbuf_overhead          =
                                 sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM;
//
// We'll allocate 2K data buffers for an inline case because this would require
// a single page per mbuf. If we used 4K data buffers here it would require 2
// pages for a single buffer (due to "mbuf_overhead") and this is a much more
// demanding memory constraint.
//
static constexpr size_t   inline_mbuf_data_size  = 2048;

//
// Size of the data buffer in the non-inline case.
//
// We may want to change (increase) this value in future, while the
// inline_mbuf_data_size value will unlikely change due to reasons described
// above.
//
static constexpr size_t   mbuf_data_size         = 2048;

// (INLINE_MBUF_DATA_SIZE(2K)*32 = 64K = Max TSO/LRO size) + 1 mbuf for headers
static constexpr uint8_t  max_frags              = 32 + 1;

//
// Intel's 40G NIC HW limit for a number of fragments in an xmit segment.
//
// See Chapter 8.4.1 "Transmit Packet in System Memory" of the xl710 devices
// spec. for more details.
//
static constexpr uint8_t  i40e_max_xmit_segment_frags = 8;

//
// VMWare's virtual NIC limit for a number of fragments in an xmit segment.
//
// see drivers/net/vmxnet3/base/vmxnet3_defs.h VMXNET3_MAX_TXD_PER_PKT
//
static constexpr uint8_t vmxnet3_max_xmit_segment_frags = 16;

static constexpr uint16_t inline_mbuf_size       =
                                inline_mbuf_data_size + mbuf_overhead;

#if 0
uint32_t qp_mempool_obj_size(bool hugetlbfs_membackend)
{
    uint32_t mp_size = 0;
    struct rte_mempool_objsz mp_obj_sz = {};

    //
    // We will align each size to huge page size because DPDK allocates
    // physically contiguous memory region for each pool object.
    //

    // Rx
    if (hugetlbfs_membackend) {
        mp_size +=
            align_up(rte_mempool_calc_obj_size(mbuf_overhead, 0, &mp_obj_sz)+
                                        sizeof(struct rte_pktmbuf_pool_private),
                                               memory::huge_page_size);
    } else {
        mp_size +=
            align_up(rte_mempool_calc_obj_size(inline_mbuf_size, 0, &mp_obj_sz)+
                                        sizeof(struct rte_pktmbuf_pool_private),
                                               memory::huge_page_size);
    }
    //Tx
    std::memset(&mp_obj_sz, 0, sizeof(mp_obj_sz));
    mp_size += align_up(rte_mempool_calc_obj_size(inline_mbuf_size, 0,
                                                  &mp_obj_sz)+
                                        sizeof(struct rte_pktmbuf_pool_private),
                                                  memory::huge_page_size);
    return mp_size;
}
#endif

static constexpr const char* pktmbuf_pool_name   = "dpdk_pktmbuf_pool";

/*
 * When doing reads from the NIC queues, use this batch size
 */
static constexpr uint8_t packet_read_size        = 32;

}; // namespace dpdk
}; // namespace lazybsd
