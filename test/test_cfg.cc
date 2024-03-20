/**
 * @file test_cfg.cc
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-02-24
 *
 * @brief 测试程序模板
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */

#include "helper.h"
#include <cstdlib>
#include <gtest/gtest.h>
#include <lazybsd.h>
#include <lazybsd_cfg.hh>
#include <string>
#include "test_main.hh"

using namespace lazybsd;

TEST(TEST_CFG, TEST_LOAD)
{
	lazybsd_cfg cfg_xml;

	std::string  config_xml =
	R"(
<?xml version="1.0" encoding="UTF-8"?>
<lazybsd>
     <dpdk class="object">
          <channel type="string">4</channel>
          <idle_sleep type="string">0</idle_sleep>
          <lcore_mask type="string">1</lcore_mask>
          <nb_bond type="string">0</nb_bond>
          <nb_vdev type="string">0</nb_vdev>
          <numa_on type="string">1</numa_on>
          <pkt_tx_delay type="string">100</pkt_tx_delay>
          <port_list type="string">0</port_list>
          <promiscuous type="string">1</promiscuous>
          <symmetric_rss type="string">0</symmetric_rss>
          <tso type="string">0</tso>
          <tx_csum_offoad_skip type="string">0</tx_csum_offoad_skip>
          <vlan_strip type="string">1</vlan_strip>
     </dpdk>
     <freebsd class="object">
          <fd_reserve type="string">1024</fd_reserve>
          <hz type="string">100</hz>
          <boot class="object">
               <kern.features.inet6 type="string">1</kern.features.inet6>
               <kern.ipc.maxsockets type="string">262144</kern.ipc.maxsockets>
               <kern.ncallout type="string">262144</kern.ncallout>
               <net.inet.tcp.syncache.bucketlimit type="string">100</net.inet.tcp.syncache.bucketlimit>
               <net.inet.tcp.syncache.hashsize type="string">4096</net.inet.tcp.syncache.hashsize>
               <net.inet.tcp.tcbhashsize type="string">65536</net.inet.tcp.tcbhashsize>
          </boot>
          <sysctl class="object">
               <kern.ipc.maxsockbuf type="string">16777216</kern.ipc.maxsockbuf>
               <kern.ipc.somaxconn type="string">32768</kern.ipc.somaxconn>
               <net.add_addr_allfibs type="string">1</net.add_addr_allfibs>
               <net.inet.ip.forwarding type="string">0</net.inet.ip.forwarding>
               <net.inet.ip.redirect type="string">0</net.inet.ip.redirect>
               <net.inet.tcp.blackhole type="string">1</net.inet.tcp.blackhole>
               <net.inet.tcp.cc.algorithm type="string">cubic</net.inet.tcp.cc.algorithm>
               <net.inet.tcp.delayed_ack type="string">1</net.inet.tcp.delayed_ack>
               <net.inet.tcp.fast_finwait2_recycle type="string">1</net.inet.tcp.fast_finwait2_recycle>
               <net.inet.tcp.functions_default type="string">freebsd</net.inet.tcp.functions_default>
               <net.inet.tcp.hpts.maxsleep type="string">51200</net.inet.tcp.hpts.maxsleep>
               <net.inet.tcp.hpts.minsleep type="string">250</net.inet.tcp.hpts.minsleep>
               <net.inet.tcp.hpts.skip_swi type="string">1</net.inet.tcp.hpts.skip_swi>
               <net.inet.tcp.msl type="string">2000</net.inet.tcp.msl>
               <net.inet.tcp.recvbuf_auto type="string">1</net.inet.tcp.recvbuf_auto>
               <net.inet.tcp.recvbuf_max type="string">16777216</net.inet.tcp.recvbuf_max>
               <net.inet.tcp.recvspace type="string">8192</net.inet.tcp.recvspace>
               <net.inet.tcp.rfc1323 type="string">1</net.inet.tcp.rfc1323>
               <net.inet.tcp.sack.enable type="string">1</net.inet.tcp.sack.enable>
               <net.inet.tcp.sendbuf_auto type="string">1</net.inet.tcp.sendbuf_auto>
               <net.inet.tcp.sendbuf_inc type="string">16384</net.inet.tcp.sendbuf_inc>
               <net.inet.tcp.sendbuf_max type="string">16777216</net.inet.tcp.sendbuf_max>
               <net.inet.tcp.sendspace type="string">16384</net.inet.tcp.sendspace>
               <net.inet.udp.blackhole type="string">1</net.inet.udp.blackhole>
               <net.inet6.icmp6.rediraccept type="string">1</net.inet6.icmp6.rediraccept>
               <net.inet6.ip6.accept_rtadv type="string">2</net.inet6.ip6.accept_rtadv>
               <net.inet6.ip6.auto_linklocal type="string">1</net.inet6.ip6.auto_linklocal>
               <net.inet6.ip6.forwarding type="string">0</net.inet6.ip6.forwarding>
               <net.link.ether.inet.maxhold type="string">5</net.link.ether.inet.maxhold>
          </sysctl>
     </freebsd>
     <pcap class="object">
          <enable type="string">0</enable>
          <savelen type="string">16777216</savelen>
          <savepath type="string">.</savepath>
          <snaplen type="string">96</snaplen>
     </pcap>
     <port0 class="object">
          <addr type="string">192.168.1.2</addr>
          <broadcast type="string">192.168.1.255</broadcast>
          <gateway type="string">192.168.1.1</gateway>
          <netmask type="string">255.255.255.0</netmask>
     </port0>
</lazybsd>
	)";

	EXPECT_EQ(cfg_xml.loadString(config_xml), EXIT_SUCCESS);
     EXPECT_EQ(cfg_xml.dpdk.nb_channel, 4);
}
