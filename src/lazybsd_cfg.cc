/**
 * @file lazybsd_cfg.cc
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-20
 *
 * @brief lazybsd configure file
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#include <cstdlib>
#include <cstring>
#include <fmt/color.h>
#include <lazybsd_cfg.hh>

#include <boost/foreach.hpp>
#include <string>

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace boost::property_tree;
using namespace std;

namespace lazybsd {
namespace cfg {

    /**
     * @brief init lazybsd cfg module
     *
     * @return true
     * @return false
     */
    bool init(void)
    {
        return EXIT_SUCCESS;
    }

    int config::ptree2struct(const ptree& cfg)
    {
        lazybsd_global_ptr()->dpdk.nb_channel          = cfg.get<int>("lazybsd.lazybsd_global_ptr()->dpdk.channel");
        lazybsd_global_ptr()->dpdk.idle_sleep          = cfg.get<int>("lazybsd.lazybsd_global_ptr()->dpdk.idle_sleep");
        lazybsd_global_ptr()->dpdk.lcore_mask          = cfg.get<string>("lazybsd.lazybsd_global_ptr()->dpdk.lcore_mask").c_str();
        lazybsd_global_ptr()->dpdk.nb_bond             = cfg.get<int>("lazybsd.lazybsd_global_ptr()->dpdk.nb_bond");
        lazybsd_global_ptr()->dpdk.nb_vdev             = cfg.get<int>("lazybsd.lazybsd_global_ptr()->dpdk.nb_vdev");
        lazybsd_global_ptr()->dpdk.numa_on             = cfg.get<int>("lazybsd.lazybsd_global_ptr()->dpdk.numa_on");
        lazybsd_global_ptr()->dpdk.pkt_tx_delay        = cfg.get<int>("lazybsd.lazybsd_global_ptr()->dpdk.pkt_tx_delay");
        lazybsd_global_ptr()->dpdk.portid_list[0]      = cfg.get<int>("lazybsd.lazybsd_global_ptr()->dpdk.port_list");
        lazybsd_global_ptr()->dpdk.promiscuous         = cfg.get<int>("lazybsd.lazybsd_global_ptr()->dpdk.promiscuous");
        lazybsd_global_ptr()->dpdk.symmetric_rss       = cfg.get<int>("lazybsd.lazybsd_global_ptr()->dpdk.symmetric_rss");
        lazybsd_global_ptr()->dpdk.tso                 = cfg.get<int>("lazybsd.lazybsd_global_ptr()->dpdk.tso");
        lazybsd_global_ptr()->dpdk.tx_csum_offoad_skip = cfg.get<int>("lazybsd.lazybsd_global_ptr()->dpdk.tx_csum_offoad_skip");
        lazybsd_global_ptr()->dpdk.vlan_strip          = cfg.get<int>("lazybsd.lazybsd_global_ptr()->dpdk.vlan_strip");

        lazybsd_global_ptr()->freebsd.hz         = cfg.get<int>("lazybsd.freebsd.hz");
        lazybsd_global_ptr()->freebsd.fd_reserve = cfg.get<int>("lazybsd.freebsd.fd_reserve");

        lazybsd_global_ptr()->pcap.enable    = cfg.get<int>("lazybsd.pcap.enable");
        lazybsd_global_ptr()->pcap.save_len  = cfg.get<int>("lazybsd.pcap.savelen");
        lazybsd_global_ptr()->pcap.save_path = cfg.get<string>("lazybsd.pcap.savepath").c_str();
        lazybsd_global_ptr()->pcap.snap_len  = cfg.get<int>("lazybsd.pcap.snaplen");

        lazybsd_global_ptr()->dpdk.port_cfgs->addr      = cfg.get<string>("lazybsd.port0.addr").c_str();
        lazybsd_global_ptr()->dpdk.port_cfgs->broadcast = cfg.get<string>("lazybsd.port0.broadcast").c_str();
        lazybsd_global_ptr()->dpdk.port_cfgs->gateway   = cfg.get<string>("lazybsd.port0.gateway").c_str();
        lazybsd_global_ptr()->dpdk.port_cfgs->netmask   = cfg.get<string>("lazybsd.port0.netmask").c_str();

        return EXIT_SUCCESS;
    }

    config::config() noexcept {}

    /**
     * @brief 加载xml配置文件
     * @return int 加载结果
     */
    int config::loadFile(const std::string cfg_file)
    {
        ptree cfg;

        if (parserFile(cfg_file, cfg) == EXIT_FAILURE) {
            fmt::print("cfg_file {} parse fail\r\n", cfg_file);
            return EXIT_FAILURE;
        }

        if (ptree2struct(cfg) == EXIT_FAILURE) {
            fmt::print("ptree2struct fail\r\n");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    /**
     * @brief  加载xml配置字符串
     * @param  cfg_string xml配置字符串
     * @return int 执行结果
     */
    int config::loadString(const std::string cfg_string)
    {
        ptree cfg;
        if (parserString(cfg_string, cfg) == EXIT_FAILURE) {
            fmt::print("cfg_string parse fail fail\r\n");
            return EXIT_FAILURE;
        }

        if (ptree2struct(cfg) == EXIT_FAILURE) {
            fmt::print("ptree2struct fail\r\n");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    int config::parserString(const std::string& stream, boost::property_tree::ptree& cfg)
    {
        std::stringstream ss(stream);
        int               result = EXIT_SUCCESS;

        try {
            xml_parser::read_xml(ss, cfg);
        } catch (const std::exception& e) {
            fmt::print("parser faile {}\r\n", e.what());
            result = EXIT_FAILURE;
        }

        return result;
    }

    int config::parserFile(const std::string& fileName, ptree& cfg)
    {
        string fileNameTag = fileName.substr(fileName.rfind(".") + 1, fileName.length());

        if (fileNameTag == "xml") {
            xml_parser::read_xml(fileName, cfg);
        } else {
            fmt::print("cannot parse file {}\r\n", fileName);
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
}  // namespace cfg
}  // namespace lazybsd