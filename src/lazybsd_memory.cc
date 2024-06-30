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
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

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

#include "lazybsd_dpdk.hh"
#include "lazybsd_cfg.hh"
#include "lazybsd_veth.hh"
#include "lazybsd_api.hh"
#include "lazybsd_memory.hh"

#define    PAGE_SIZE            4096
#define    PAGE_SHIFT            12
#define    PAGE_MASK            (PAGE_SIZE - 1)
#define    trunc_page(x)        ((x) & ~PAGE_MASK)
#define    round_page(x)        (((x) + PAGE_MASK) & ~PAGE_MASK)

//struct lazybsd_tx_offload;

// lazybsd_ref_pool allocate rte_mbuf without data space, which data point to bsd mbuf's data address.
static struct rte_mempool *lazybsd_ref_pool[NB_SOCKETS];

#define    Head_INC(h)    {\
    if ( ++h >= TX_QUEUE_SIZE ) \
        h = 0;\
    };

#define    Head_DEC(h)    do{\
    if ( --h < 0 ) \
        h = TX_QUEUE_SIZE-1;\
    }while(0);

#ifdef __cplusplus
extern "C" {
#endif

// bsd mbuf was moved into nic_tx_ring from tmp_tables, after rte_eth_tx_burst() succeed.
static struct mbuf_txring nic_tx_ring[RTE_MAX_ETHPORTS];
static inline int lazybsd_txring_enqueue(struct mbuf_txring* q, void *p, int seg_num);
static inline void lazybsd_txring_init(struct mbuf_txring* r, uint32_t len);

struct lcore_conf lcore_conf;
struct rte_mempool *pktmbuf_pool[NB_SOCKETS];

typedef struct _list_manager_s
{
    uint64_t    *ele;
    int        size;
    //int        FreeNum;
    int     top;
}StackList_t;

static StackList_t         lazybsd_mpage_ctl = {0};
static uint64_t             lazybsd_page_start = (uint64_t)NULL, lazybsd_page_end = (uint64_t)NULL;
static phys_addr_t        *lazybsd_mpage_phy = NULL;

static inline void        *stklist_pop(StackList_t *p);
static inline int         stklist_push(StackList_t * p, uint64_t val);

static int                 stklist_init(StackList_t*p, int size)
{

    int i = 0;

    if (p==NULL || size<=0){
        return -1;
    }
    p->size = size;
    p->top = 0;
    if ( posix_memalign((void**)&p->ele, sizeof(uint64_t), sizeof(uint64_t)*size) != 0)
        return -2;

    return 0;
}

static inline void *stklist_pop(StackList_t *p)
{
    int head = 0;

    if (p==NULL)
        return NULL;

    if (p->top > 0 ){
        return (void*)p->ele[--p->top];
    }
    else
        return NULL;
}

//id: the id of element to be freed.
//return code: -1: faile;  >=0:OK.
static inline int stklist_push(StackList_t *p,  const uint64_t val){
    int tail = 0;

    if (p==NULL)
        return -1;
    if (p->top < p->size){
        p->ele[p->top++] = val;
        return 0;
    }
    else
        return -1;
}

static inline int stklist_size(StackList_t * p)
{
    return p->size;
}

// set (void*) to rte_mbuf's priv_data.
static inline int lazybsd_mbuf_set_uint64(struct rte_mbuf* p, uint64_t data)
{
    if (rte_pktmbuf_priv_size(p->pool) >= sizeof(uint64_t))
        *((uint64_t*)(p+1)) = data;
    return 0;
}

/*************************
* if mbuf has num segment in all, Dev's sw_ring will use num descriptions. lazybsd_txring also use num segments as below:
* <---     num-1          ---->|ptr| head |
* ----------------------------------------------
* | 0 | 0 | ..............| 0  | p | XXX  |
*-----------------------------------------------
*************************/
static inline int lazybsd_txring_enqueue(struct mbuf_txring* q, void *p, int seg_num)
{
    int i = 0;
    for ( i=0; i<seg_num-1; i++){
        if ( q->m_table[q->head] ){
            lazybsd_mbuf_free(q->m_table[q->head]);
            q->m_table[q->head] = NULL;
        }
        Head_INC(q->head);
    }
    if ( q->m_table[q->head] )
        lazybsd_mbuf_free(q->m_table[q->head]);
    q->m_table[q->head] = p;
    Head_INC(q->head);

    return 0;
}

// pop out from head-1 .
static inline int lazybsd_txring_pop(struct mbuf_txring* q, int num)
{
    int i = 0;

    for (i=0; i<num; i++){
        Head_DEC(q->head);
        if ( (i==0 && q->m_table[q->head]==NULL) || (i>0 && q->m_table[q->head]!=NULL) ){
            rte_panic("lazybsd_txring_pop fatal error!");
        }
        if ( q->m_table[q->head] != NULL ){
            lazybsd_mbuf_free(q->m_table[q->head]);
            q->m_table[q->head] = NULL;
        }
    }
}

static inline void lazybsd_txring_init(struct mbuf_txring* q, uint32_t num)
{
    memset(q, 0, sizeof(struct mbuf_txring)*num);
}

void lazybsd_init_ref_pool(int nb_mbuf, int socketid)
{
    char s[64] = {0};

    if (lazybsd_ref_pool[socketid] != NULL) {
            return;
    }
    snprintf(s, sizeof(s), "lazybsd_ref_pool_%d", socketid);
    if (rte_eal_process_type() == RTE_PROC_PRIMARY) {
        lazybsd_ref_pool[socketid] = rte_pktmbuf_pool_create(s, nb_mbuf, MEMPOOL_CACHE_SIZE, 0, 0, socketid);
    } else {
        lazybsd_ref_pool[socketid] = rte_mempool_lookup(s);
    }
}

int lazybsd_mmap_init()
{
    int err = 0;
    int i = 0;
    uint64_t    virt_addr = (uint64_t)NULL;
    phys_addr_t    phys_addr = 0;
    uint64_t    bsd_memsz = (lazybsd_global_cfg.freebsd.mem_size << 20);
    unsigned int bsd_pagesz = 0;

    lazybsd_page_start = (uint64_t)mmap( NULL, bsd_memsz, PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_POPULATE, -1, 0);
    if (lazybsd_page_start == (uint64_t)-1){
        rte_panic("lazybsd_mmap_init get lazybsd_page_start failed, err=%d.\n", errno);
        return -1;
    }

    if ( mlock((void*)lazybsd_page_start, bsd_memsz)<0 )    {
        rte_panic("mlock failed, err=%d.\n", errno);
        return -1;
    }
    lazybsd_page_end = lazybsd_page_start + bsd_memsz;
    bsd_pagesz = (bsd_memsz>>12);
    rte_log(RTE_LOG_INFO, RTE_LOGTYPE_USER1, "lazybsd_mmap_init mmap %d pages, %d MB.\n", bsd_pagesz, lazybsd_global_cfg.freebsd.mem_size);
    printf("lazybsd_mmap_init mem[0x%lx:0x%lx]\n", lazybsd_page_start, lazybsd_page_end);

    if (posix_memalign((void**)&lazybsd_mpage_phy, sizeof(phys_addr_t), bsd_pagesz*sizeof(phys_addr_t))!=0){
        rte_panic("posix_memalign get lazybsd_mpage_phy failed, err=%d.\n", errno);
        return -1;
    }

    stklist_init(&lazybsd_mpage_ctl, bsd_pagesz);

    for (i=0; i<bsd_pagesz; i++ ){
        virt_addr = lazybsd_page_start + PAGE_SIZE*i;
        memset((void*)virt_addr, 0, PAGE_SIZE);

        stklist_push( &lazybsd_mpage_ctl, virt_addr);
        lazybsd_mpage_phy[i] = rte_mem_virt2phy((const void*)virt_addr);
        if ( lazybsd_mpage_phy[i] == RTE_BAD_IOVA ){
            rte_panic("rte_mem_virt2phy return invalid address.");
            return -1;
        }
    }

    lazybsd_txring_init(&nic_tx_ring[0], RTE_MAX_ETHPORTS);

    return 0;
}

// 1: vma in fstack page table;  0: vma not in fstack pages, in DPDK pool.
static inline int lazybsd_chk_vma(const uint64_t virtaddr)
{
    return  !!( virtaddr > lazybsd_page_start && virtaddr < lazybsd_page_end );
}

/*
 * Get physical address of any mapped virtual address in the current process.
 */
static inline uint64_t lazybsd_mem_virt2phy(const void* virtaddr)
{
    uint64_t    addr = 0;
    uint32_t    pages = 0;

    pages = (((uint64_t)virtaddr - (uint64_t)lazybsd_page_start)>>PAGE_SHIFT);
    if (pages >= stklist_size(&lazybsd_mpage_ctl)){
        rte_panic("lazybsd_mbuf_virt2phy get invalid pages %d.", pages);
        return -1;
    }

    addr = lazybsd_mpage_phy[pages] + ((const uint64_t)virtaddr & PAGE_MASK);
    return addr;
}

void *lazybsd_mem_get_page()
{
    return (void*)stklist_pop(&lazybsd_mpage_ctl);
}

int    lazybsd_mem_free_addr(void *p)
{
    stklist_push(&lazybsd_mpage_ctl, (const uint64_t)p);
    return 0;
}

static inline void lazybsd_offload_set(struct lazybsd_dpdk_if_context *ctx, void *m, struct rte_mbuf *head)
{
    void                    *data = NULL;
    struct lazybsd_tx_offload     offload = {0};

    lazybsd_mbuf_tx_offload(m, &offload);
    data = rte_pktmbuf_mtod(head, void*);

    if (offload.ip_csum) {
        /* ipv6 not supported yet */
        struct rte_ipv4_hdr *iph;
        int iph_len;
        iph = (struct rte_ipv4_hdr *)(data + RTE_ETHER_HDR_LEN);
        iph_len = (iph->version_ihl & 0x0f) << 2;

        head->ol_flags |= RTE_MBUF_F_TX_TCP_CKSUM | RTE_MBUF_F_TX_IPV4;
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
            head->ol_flags |= RTE_MBUF_F_TX_TCP_CKSUM;
            head->l2_len = RTE_ETHER_HDR_LEN;
            head->l3_len = iph_len;
        }
    }
}

// create rte_buf refer to data which is transmit from bsd stack by EXT_CLUSTER.
static inline struct rte_mbuf*     lazybsd_extcl_to_rte(void *m )
{
    struct rte_mempool *mbuf_pool = pktmbuf_pool[lcore_conf.socket_id];
    struct rte_mbuf *src_mbuf = NULL;
    struct rte_mbuf *p_head = NULL;

    src_mbuf = (struct rte_mbuf*)lazybsd_rte_frm_extcl(m);
    if ( NULL==src_mbuf ){
        return NULL;
    }
    p_head = rte_pktmbuf_clone(src_mbuf, mbuf_pool);
    if (p_head == NULL){
        return NULL;
    }

    return p_head;
}

//  create rte_mbuf refer to data in bsd mbuf.
static inline struct rte_mbuf*     lazybsd_bsd_to_rte(void *m, int total)
{
    struct rte_mempool *mbuf_pool = lazybsd_ref_pool[lcore_conf.socket_id];
    struct rte_mbuf *p_head = NULL;
    struct rte_mbuf *cur = NULL, *prev = NULL, *tmp=NULL;
    void    *data = NULL;
    void    *p_bsdbuf = NULL;
    unsigned len = 0;

    p_head = rte_pktmbuf_alloc(mbuf_pool);
    if (p_head == NULL){
        return NULL;
    }
    p_head->pkt_len = total;
    p_head->nb_segs = 0;
    cur = p_head;
    p_bsdbuf = m;
    while ( p_bsdbuf ){
        if (cur == NULL) {
            cur = rte_pktmbuf_alloc(mbuf_pool);
            if (cur == NULL) {
                rte_pktmbuf_free(p_head);
                return NULL;
            }
        }
        lazybsd_next_mbuf(&p_bsdbuf, &data, &len);        // p_bsdbuf move to next mbuf.
        cur->buf_addr = data;
        cur->buf_iova = lazybsd_mem_virt2phy((const void*)(cur->buf_addr));
        cur->data_off = 0;
        cur->data_len = len;

        p_head->nb_segs++;
        if (prev != NULL) {
            prev->next = cur;
        }
        prev = cur;
        cur = NULL;
    }

    return p_head;
}

int lazybsd_if_send_onepkt(struct lazybsd_dpdk_if_context *ctx, void *m, int total)
{
    struct rte_mbuf *head = NULL;
    void            *src_buf = NULL;
    void            *p_data = NULL;
    struct lcore_conf *qconf = NULL;
    unsigned        len = 0;

    if ( !m ){
        rte_log(RTE_LOG_CRIT, RTE_LOGTYPE_USER1, "lazybsd_dpdk_if_send_ex input invalid NULL address.");
        return 0;
    }
    p_data = lazybsd_mbuf_mtod(m);
    if ( lazybsd_chk_vma((uint64_t)p_data)){
        head = lazybsd_bsd_to_rte(m, total);
    }
    else if ( (head = lazybsd_extcl_to_rte(m)) == NULL ){
           rte_panic("data address 0x%lx is out of page bound or not malloced by DPDK recver.", (uint64_t)p_data);
        return 0;
    }

    if (head == NULL){
        rte_log(RTE_LOG_CRIT, RTE_LOGTYPE_USER1, "lazybsd_if_send_onepkt call lazybsd_bsd_to_rte failed.");
        lazybsd_mbuf_free(m);
        return 0;
    }

    lazybsd_offload_set(ctx, m, head);
    qconf = &lcore_conf;
    len = qconf->tx_mbufs[ctx->port_id].len;
    qconf->tx_mbufs[ctx->port_id].m_table[len] = head;
    qconf->tx_mbufs[ctx->port_id].bsd_m_table[len] = m;
    len++;

    return len;
}

int lazybsd_enq_tx_bsdmbuf(uint8_t portid, void *p_mbuf, int nb_segs)
{
    return lazybsd_txring_enqueue(&nic_tx_ring[portid], p_mbuf, nb_segs);
}

#ifdef __cplusplus
}
#endif
