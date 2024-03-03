/**
 * @file lazybsd_cfg.h
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-20
 *
 * @brief
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#include <cstddef>
#include <cstdint>
#include <lazybsd.h>
#include <string>
#include <array>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#ifndef __LAZYBSD_CFG__
#define __LAZYBSD_CFG__

namespace lazybsd {

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

constexpr size_t MAC_SIZE = 6;

struct lazybsd_hw_features {
    uint8_t rx_csum;
    uint8_t rx_lro;
    uint8_t tx_csum_ip;
    uint8_t tx_csum_l4;
    uint8_t tx_tso;
};

struct lazybsd_port_cfg {
    std::string name;
    std::string ifname;
    uint8_t port_id;
    std::array<uint8_t, MAC_SIZE> mac;
    struct lazybsd_hw_features hw_features;
    std::string addr;
    std::string netmask;
    std::string broadcast;
    std::string gateway;

    std::string vip_ifname;
    std::string vip_addr_str;
    std::string *vip_addr_array;
    uint32_t nb_vip;

#ifdef INET6
    std::string addr6_str;
    std::string gateway6_str;
    uint8_t prefix_len;

    std::string vip_addr6_str;
    std::string *vip_addr6_array;
    uint32_t nb_vip6;
    uint8_t vip_prefix_len;
#endif

    int nb_lcores;
    int nb_slaves;
    std::array<uint16_t, DPDK_MAX_LCORE> lcore_list;
    uint16_t *slave_portid_list;
};

struct lazybsd_vdev_cfg {
    std::string name;
    std::string iface;
    std::string path;
    std::string mac;
    uint8_t vdev_id;
    uint8_t nb_queues;
    uint8_t nb_cq;
    uint16_t queue_size;
};

struct lazybsd_bond_cfg {
    std::string name;
    std::string slave;
    std::string primary;
    std::string bond_mac;
    std::string xmit_policy;
    uint8_t bond_id;
    uint8_t mode;
    uint8_t socket_id;
    uint8_t lsc_poll_period_ms;
    uint16_t up_delay;
    uint16_t down_delay;
};

struct lazybsd_freebsd_cfg {
    std::string name;
    std::string str;
    void *value;
    size_t vlen;
};

enum {
    PORT_PRIME,
    PORT_PROXY,
    PORT_THIRD,
    PORT_SIZE
};

struct lazybsd_cfg {
private:
    int parserFile(const std::string & fileName, boost::property_tree::ptree& cfg);
    int parserString(const std::string & stream, boost::property_tree::ptree& cfg);
    int ptree2struct(const boost::property_tree::ptree& cfg);

public:

    lazybsd_cfg() noexcept;
	~lazybsd_cfg() = default;

	/**
	* @brief  加载toml配置文件
	* @param  cfg_file json/xml配置文件
	* @return int 执行结果
	*/
	int loadFile(const std::string cfg_file);

    /**
	* @brief  加载toml配置文件
	* @param  cfg_string json/xml配置字符串
	* @return int 执行结果
	*/
	int loadString(const std::string cfg_string);

	/**
	* @brief 打印配置文件
	*/
	void print();

    struct {
        /* primary proxy */
        std::string proc_type;

        /* mask of enabled lcores */
        std::string lcore_mask;

        /* mask of current proc on all lcores */
        std::string proc_mask;

        /* specify base virtual address to map. */
        std::string base_virtaddr;

        /* allow processes that do not want to co-operate to have different memory regions */
        std::string file_prefix;

        /* load an external driver */
        std::string pci_whitelist;

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
        uint16_t portid_list;

        // load dpdk log level
        uint16_t log_level;
        // MAP(portid => struct lazybsd_port_cfg*)
        struct lazybsd_port_cfg port_cfgs;
        struct lazybsd_vdev_cfg vdev_cfgs;
        struct lazybsd_bond_cfg bond_cfgs;
    } dpdk;

    struct {
        int enable;
        int type;
        std::string kni_action;
        std::string method;
        std::string tcp_port;
        std::string udp_port;
    } kni;

    struct {
        int level;
        std::string dir;
    } log;

    struct {
        std::vector<lazybsd_freebsd_cfg> boot;
        std::vector<lazybsd_freebsd_cfg> sysctl;
        long physmem;
        int hz;
        int fd_reserve;
        int mem_size;
    } freebsd;

    struct {
        uint16_t enable;
        uint16_t snap_len;
        uint32_t save_len;
        std::string save_path;
    } pcap;

    struct {
        std::string addr;
        std::string broadcast;
        std::string gateway;
        std::string netmask;
    } port;
};

};

lazybsd::lazybsd_cfg& lazybsd_cfg_runtime(void);

#endif // __LAZYBSD_CFG__