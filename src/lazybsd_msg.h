/**
 * @file lazybsd_msg.h
 * @author mengdemao (mengdemao19951021@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef _LAZYBSD_MSG_H_
#define _LAZYBSD_MSG_H_

#include <rte_memory.h>

#define LAZYBSD_MSG_RING_IN  "lazybsd_msg_ring_in_"
#define LAZYBSD_MSG_RING_OUT "lazybsd_msg_ring_out_"
#define LAZYBSD_MSG_POOL     "lazybsd_msg_pool"

/* MSG TYPE: sysctl, ioctl, etc.. */
enum LAZYBSD_MSG_TYPE {
    LAZYBSD_UNKNOWN = 0,
    LAZYBSD_SYSCTL,
    LAZYBSD_IOCTL,
    LAZYBSD_IOCTL6,
    LAZYBSD_ROUTE,
    LAZYBSD_TOP,
    LAZYBSD_NGCTL,
    LAZYBSD_IPFW_CTL,
    LAZYBSD_TRAFFIC,
    LAZYBSD_KNICTL,

    /*
     * to add other msg type before LAZYBSD_MSG_NUM
     */
    LAZYBSD_MSG_NUM,
};

struct lazybsd_sysctl_args {
    int *name;
    unsigned namelen;
    void *oldp;
    size_t *oldlenp;
    void *newp;
    size_t newlen;
};

struct lazybsd_ioctl_args {
    unsigned long cmd;
    void *data;
};

struct lazybsd_route_args {
    int fib;
    unsigned len;
    unsigned maxlen;
    void *data;
};

struct lazybsd_top_args {
    unsigned long loops;
    unsigned long idle_tsc;
    unsigned long work_tsc;
    unsigned long sys_tsc;
    unsigned long usr_tsc;
};

struct lazybsd_ngctl_args {
    int cmd;
    int ret;
    void *data;
};

enum LAZYBSD_IPFW_CMD {
    LAZYBSD_IPFW_GET,
    LAZYBSD_IPFW_SET,
};

struct lazybsd_ipfw_args {
    int cmd;
    int level;
    int optname;
    void *optval;
    socklen_t *optlen;
};

struct lazybsd_traffic_args {
    uint64_t rx_packets;
    uint64_t rx_bytes;
    uint64_t tx_packets;
    uint64_t tx_bytes;
};

enum LAZYBSD_KNICTL_CMD {
    LAZYBSD_KNICTL_CMD_GET,
    LAZYBSD_KNICTL_CMD_SET,
    LAZYBSD_KNICTL_CMD_UNKNOWN,
};

enum LAZYBSD_KNICTL_ACTION {
    LAZYBSD_KNICTL_ACTION_DEFAULT,
    LAZYBSD_KNICTL_ACTION_ALL_TO_KNI,
    LAZYBSD_KNICTL_ACTION_ALL_TO_FF,
    LAZYBSD_KNICTL_ACTION_MAX
};

struct lazybsd_knictl_args {
    int kni_cmd;
    int kni_action;
};


#define MAX_MSG_BUF_SIZE 10240

/* structure of ipc msg */
struct lazybsd_msg {
    enum LAZYBSD_MSG_TYPE msg_type;
    /* Result of msg processing */
    int result;
    /* Length of segment buffer. */
    size_t buf_len;
    /* Address of segment buffer. */
    char *buf_addr;
    char *original_buf;
    size_t original_buf_len;

    union {
        struct lazybsd_sysctl_args sysctl;
        struct lazybsd_ioctl_args ioctl;
        struct lazybsd_route_args route;
        struct lazybsd_top_args top;
        struct lazybsd_ngctl_args ngctl;
        struct lazybsd_ipfw_args ipfw;
        struct lazybsd_traffic_args traffic;
        struct lazybsd_knictl_args knictl;
    };
} __attribute__((packed)) __rte_cache_aligned;

#endif
