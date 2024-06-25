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
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace boost::property_tree;
using namespace std;

namespace lazybsd {

int lazybsd_cfg::ptree2struct(const ptree& cfg)
{
	dpdk.nb_channel = cfg.get<int>("lazybsd.dpdk.channel");
	dpdk.idle_sleep = cfg.get<int>("lazybsd.dpdk.idle_sleep");
	dpdk.lcore_mask = cfg.get<string>("lazybsd.dpdk.lcore_mask");
	dpdk.nb_bond = cfg.get<int>("lazybsd.dpdk.nb_bond");
	dpdk.nb_vdev = cfg.get<int>("lazybsd.dpdk.nb_vdev");
	dpdk.numa_on = cfg.get<int>("lazybsd.dpdk.numa_on");
	dpdk.pkt_tx_delay = cfg.get<int>("lazybsd.dpdk.pkt_tx_delay");
	dpdk.portid_list[0] = cfg.get<int>("lazybsd.dpdk.port_list");
	dpdk.promiscuous = cfg.get<int>("lazybsd.dpdk.promiscuous");
	dpdk.symmetric_rss = cfg.get<int>("lazybsd.dpdk.symmetric_rss");
	dpdk.tso = cfg.get<int>("lazybsd.dpdk.tso");
	dpdk.tx_csum_offoad_skip = cfg.get<int>("lazybsd.dpdk.tx_csum_offoad_skip");
	dpdk.vlan_strip = cfg.get<int>("lazybsd.dpdk.vlan_strip");

	freebsd.hz = cfg.get<int>("lazybsd.freebsd.hz");
	freebsd.fd_reserve = cfg.get<int>("lazybsd.freebsd.fd_reserve");

	pcap.enable = cfg.get<int>("lazybsd.pcap.enable");
	pcap.save_len = cfg.get<int>("lazybsd.pcap.savelen");
	pcap.save_path = cfg.get<string>("lazybsd.pcap.savepath");
	pcap.snap_len = cfg.get<int>("lazybsd.pcap.snaplen");

	port.addr = cfg.get<string>("lazybsd.port0.addr");
	port.broadcast = cfg.get<string>("lazybsd.port0.broadcast");
	port.gateway = cfg.get<string>("lazybsd.port0.gateway");
	port.netmask = cfg.get<string>("lazybsd.port0.netmask");

	return EXIT_SUCCESS;
}

lazybsd_cfg::lazybsd_cfg() noexcept
{
}

/**
 * @brief 加载xml配置文件
 * @return int 加载结果
 */
int lazybsd_cfg::loadFile(const std::string cfg_file)
{
	ptree cfg;

	if (parserFile(cfg_file, cfg) == EXIT_FAILURE) {
		fmt::print("cfg_file {} parse fail\r\n", cfg_file);
		return EXIT_FAILURE;
	}

	if (ptree2struct(cfg) == EXIT_FAILURE)
	{
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
int lazybsd_cfg::loadString(const std::string cfg_string)
{
	ptree cfg;
	if (parserString(cfg_string, cfg) == EXIT_FAILURE)
	{
		fmt::print("cfg_string parse fail fail\r\n");
		return EXIT_FAILURE;
	}

	if (ptree2struct(cfg) == EXIT_FAILURE)
	{
		fmt::print("ptree2struct fail\r\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/**
 * @brief 打印配置参数
 */
void lazybsd_cfg::print(void)
{

}

int lazybsd_cfg::parserString(const std::string & stream, boost::property_tree::ptree& cfg)
{
	std::stringstream ss(stream);
	int result = EXIT_SUCCESS;

	try {
		xml_parser::read_xml(ss, cfg);
	}
	catch (const std::exception& e) {
		fmt::print("parser faile {}\r\n", e.what());
		result = EXIT_FAILURE;
	}

	return result;
}

int lazybsd_cfg::parserFile(const std::string &fileName, ptree& cfg)
{
	string fileNameTag = fileName.substr(fileName.rfind(".") + 1,fileName.length());

	if (fileNameTag == "xml") {
		xml_parser::read_xml(fileName, cfg);
	} else {
		fmt::print("cannot parse file {}\r\n", fileName);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

lazybsd_cfg lazybsd_global_cfg;

}
