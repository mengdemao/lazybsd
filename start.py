#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
lazybsd 启动脚本
功能：
1. 分析 CPU 逻辑核心与内存通道 (NUMA 拓扑)
2. 解析并打印 config.xml 配置
3. 执行 lazybsd_runner 可执行文件
"""

import os
import sys
import subprocess
import xml.etree.ElementTree as ET
import re
import json
from pathlib import Path

class Colors:
    """终端颜色输出"""
    HEADER = '\033[95m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    GREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'


def print_section(title):
    """打印带格式的章节标题"""
    print(f"\n{Colors.HEADER}{'='*60}{Colors.ENDC}")
    print(f"{Colors.BOLD}{Colors.CYAN}{title}{Colors.ENDC}")
    print(f"{Colors.HEADER}{'='*60}{Colors.ENDC}")


def print_item(key, value, indent=0):
    """打印配置项"""
    prefix = "  " * indent
    print(f"{prefix}{Colors.GREEN}{key}:{Colors.ENDC} {value}")


def get_cpu_info():
    """获取 CPU 逻辑核心与物理信息"""
    cpu_info = {
        "logical_cores": 0,
        "physical_cores": 0,
        "sockets": 0,
        "numa_nodes": [],
        "cpu_topology": []
    }

    try:
        # 获取逻辑核心数
        result = subprocess.run(['nproc'], capture_output=True, text=True)
        if result.returncode == 0:
            cpu_info["logical_cores"] = int(result.stdout.strip())

        # 获取 lscpu 信息
        result = subprocess.run(['lscpu'], capture_output=True, text=True)
        if result.returncode == 0:
            lscpu_output = result.stdout

            # 解析 Socket 数量
            socket_match = re.search(r'Socket\(s\):\s+(\d+)', lscpu_output)
            if socket_match:
                cpu_info["sockets"] = int(socket_match.group(1))

            # 解析每核心线程数
            threads_match = re.search(r'Thread\(s\) per core:\s+(\d+)', lscpu_output)
            threads_per_core = int(threads_match.group(1)) if threads_match else 1

            # 解析每 Socket 核心数
            cores_match = re.search(r'Core\(s\) per socket:\s+(\d+)', lscpu_output)
            if cores_match:
                cores_per_socket = int(cores_match.group(1))
                cpu_info["physical_cores"] = cores_per_socket * cpu_info["sockets"]

        # 获取 NUMA 信息
        result = subprocess.run(['numactl', '--hardware'], capture_output=True, text=True)
        if result.returncode == 0:
            numa_output = result.stdout

            # 解析 NUMA 节点
            node_pattern = r'available:\s+(\d+)\s+nodes\s+\((.*?)\)'
            match = re.search(node_pattern, numa_output, re.DOTALL)
            if match:
                num_nodes = int(match.group(1))

                for node_id in range(num_nodes):
                    node_info = {"node": node_id, "cpus": [], "memory": {}}

                    # 解析该节点的 CPU 列表
                    cpu_pattern = rf'node\s+{node_id}\s+cpus:\s+([0-9\s]+)'
                    cpu_match = re.search(cpu_pattern, numa_output)
                    if cpu_match:
                        cpus = [int(x) for x in cpu_match.group(1).strip().split()]
                        node_info["cpus"] = cpus

                    # 解析该节点的内存信息
                    mem_pattern = rf'node\s+{node_id}\s+size:\s+(\d+)\s+MB'
                    mem_match = re.search(mem_pattern, numa_output)
                    if mem_match:
                        node_info["memory"]["size_mb"] = int(mem_match.group(1))

                    free_pattern = rf'node\s+{node_id}\s+free:\s+(\d+)\s+MB'
                    free_match = re.search(free_pattern, numa_output)
                    if free_match:
                        node_info["memory"]["free_mb"] = int(free_match.group(1))

                    cpu_info["numa_nodes"].append(node_info)

        # 获取 CPU 频率信息
        result = subprocess.run(['cat', '/proc/cpuinfo'], capture_output=True, text=True)
        if result.returncode == 0:
            cpuinfo = result.stdout
            model_match = re.search(r'model name\s+:\s+(.*)', cpuinfo)
            if model_match:
                cpu_info["model"] = model_match.group(1).strip()

            # 获取每个逻辑核心的频率
            freqs = re.findall(r'cpu MHz\s+:\s+(\d+\.\d+)', cpuinfo)
            if freqs:
                cpu_info["frequencies_mhz"] = [float(f) for f in freqs[:cpu_info["logical_cores"]]]

    except Exception as e:
        print(f"{Colors.WARNING}Warning: 获取 CPU 信息失败: {e}{Colors.ENDC}")

    return cpu_info


def get_memory_channels():
    """分析内存通道信息"""
    memory_info = {
        "total_memory_gb": 0,
        "channels": [],
        "dimms": []
    }

    try:
        # 获取总内存
        result = subprocess.run(['free', '-g'], capture_output=True, text=True)
        if result.returncode == 0:
            lines = result.stdout.strip().split('\n')
            if len(lines) > 1:
                mem_line = lines[1].split()
                if len(mem_line) > 1:
                    memory_info["total_memory_gb"] = int(mem_line[1])

        # 尝试获取 dmidecode 内存信息 (需要 root)
        result = subprocess.run(['sudo', 'dmidecode', '-t', 'memory'],
                              capture_output=True, text=True)
        if result.returncode == 0:
            dmidecode_output = result.stdout

            # 解析内存通道
            locator_pattern = r'Locator:\s+(.*)'
            locators = re.findall(locator_pattern, dmidecode_output)

            # 解析内存速度
            speed_pattern = r'Speed:\s+(\d+\s+MT/s|Unknown)'
            speeds = re.findall(speed_pattern, dmidecode_output)

            # 解析内存大小
            size_pattern = r'Size:\s+(\d+\s+GB|\d+\s+MB|No Module Installed)'
            sizes = re.findall(size_pattern, dmidecode_output)

            memory_info["dimms"] = list(zip(locators, sizes, speeds))

            # 推断通道数 (基于 DIMM 位置)
            channels = set()
            for locator in locators:
                if 'CHANNEL' in locator.upper() or 'CH_' in locator.upper():
                    ch_match = re.search(r'[Cc][Hh][_-]?(\w)', locator)
                    if ch_match:
                        channels.add(ch_match.group(1))
                elif '_A' in locator or '_B' in locator or '_C' in locator or '_D' in locator:
                    ch_match = re.search(r'_([A-D])', locator)
                    if ch_match:
                        channels.add(ch_match.group(1))

            memory_info["channels"] = sorted(list(channels))
            memory_info["channel_count"] = len(channels) if channels else "Unknown"

    except Exception as e:
        print(f"{Colors.WARNING}Warning: 获取内存通道信息失败: {e}{Colors.ENDC}")

    return memory_info


def analyze_cpu_and_memory():
    """分析 CPU 和内存配置"""
    print_section("CPU 与内存拓扑分析")

    # CPU 信息
    cpu_info = get_cpu_info()

    print(f"\n{Colors.BLUE}[CPU 基本信息]{Colors.ENDC}")
    print_item("CPU 型号", cpu_info.get("model", "Unknown"))
    print_item("逻辑核心数 (Logical Cores)", cpu_info["logical_cores"])
    print_item("物理核心数 (Physical Cores)", cpu_info["physical_cores"])
    print_item("CPU Socket 数量", cpu_info["sockets"])

    if cpu_info.get("frequencies_mhz"):
        avg_freq = sum(cpu_info["frequencies_mhz"]) / len(cpu_info["frequencies_mhz"])
        print_item("平均频率", f"{avg_freq:.2f} MHz")

    # NUMA 节点信息
    if cpu_info["numa_nodes"]:
        print(f"\n{Colors.BLUE}[NUMA 拓扑]{Colors.ENDC}")
        for node in cpu_info["numa_nodes"]:
            print(f"\n  {Colors.CYAN}NUMA Node {node['node']}:{Colors.ENDC}")
            print_item("CPU 列表", f"{node['cpus'][:8]}{'...' if len(node['cpus']) > 8 else ''}", indent=2)
            if node.get("memory"):
                size_gb = node["memory"].get("size_mb", 0) / 1024
                free_gb = node["memory"].get("free_mb", 0) / 1024
                print_item("内存总量", f"{size_gb:.2f} GB", indent=2)
                print_item("可用内存", f"{free_gb:.2f} GB", indent=2)

    # 内存通道信息
    memory_info = get_memory_channels()

    print(f"\n{Colors.BLUE}[内存通道信息]{Colors.ENDC}")
    print_item("总内存", f"{memory_info['total_memory_gb']} GB")
    print_item("内存通道数", memory_info.get("channel_count", "Unknown"))

    if memory_info["channels"]:
        print_item("通道标识", ", ".join(memory_info["channels"]))

    # 针对 DPDK 的优化建议
    print(f"\n{Colors.BLUE}[DPDK 优化建议]{Colors.ENDC}")

    if cpu_info["numa_nodes"]:
        for node in cpu_info["numa_nodes"]:
            cores = node['cpus']
            if len(cores) >= 2:
                print(f"  • NUMA Node {node['node']}: 推荐使用核心 {cores[1]} 作为 DPDK 主核心")
                print(f"    可用作数据平面的核心: {cores[2:6] if len(cores) > 6 else cores[2:]}")

    return cpu_info, memory_info


def parse_config_xml(config_path="config.xml"):
    """解析 lazybsd 配置文件"""
    print_section("配置文件解析 (config.xml)")

    if not os.path.exists(config_path):
        print(f"{Colors.FAIL}错误: 配置文件 {config_path} 不存在{Colors.ENDC}")
        print(f"{Colors.WARNING}将在当前目录搜索 config.xml...{Colors.ENDC}")

        # 搜索可能的配置文件
        possible_paths = [
            "config.xml",
            "./config.xml",
            "../config.xml",
            "/etc/lazybsd/config.xml",
            "conf/config.xml"
        ]

        found = False
        for path in possible_paths:
            if os.path.exists(path):
                config_path = path
                found = True
                print(f"{Colors.GREEN}找到配置文件: {path}{Colors.ENDC}")
                break

        if not found:
            print(f"{Colors.FAIL}未找到任何配置文件{Colors.ENDC}")
            return None

    try:
        tree = ET.parse(config_path)
        root = tree.getroot()

        print(f"{Colors.GREEN}成功加载配置文件: {config_path}{Colors.ENDC}\n")

        config_data = {}

        # 递归打印 XML 元素
        def print_element(element, indent=0):
            prefix = "  " * indent

            # 获取标签名 (去除命名空间)
            tag = element.tag.split('}')[-1] if '}' in element.tag else element.tag

            # 如果有属性，打印属性
            attrs = []
            if element.attrib:
                for k, v in element.attrib.items():
                    k_clean = k.split('}')[-1] if '}' in k else k
                    attrs.append(f"{k_clean}={v}")

            # 如果有文本内容且不为空
            text = element.text.strip() if element.text and element.text.strip() else None

            if attrs and text:
                print(f"{prefix}{Colors.CYAN}{tag}{Colors.ENDC}: {Colors.GREEN}{text}{Colors.ENDC} ({', '.join(attrs)})")
            elif attrs:
                print(f"{prefix}{Colors.CYAN}{tag}{Colors.ENDC}: {Colors.WARNING}{', '.join(attrs)}{Colors.ENDC}")
            elif text:
                print(f"{prefix}{Colors.CYAN}{tag}{Colors.ENDC}: {text}")
            else:
                print(f"{prefix}{Colors.CYAN}{tag}{Colors.ENDC}")

            # 存储到字典
            if tag not in config_data:
                config_data[tag] = []

            node_data = {
                "attributes": element.attrib,
                "text": text,
                "children": []
            }

            # 递归处理子元素
            for child in element:
                child_data = print_element(child, indent + 1)
                node_data["children"].append(child_data)

            config_data[tag].append(node_data)
            return node_data

        # 打印整个 XML 树
        print_element(root)

        # 特别解析 DPDK 相关配置
        print(f"\n{Colors.BLUE}[DPDK 关键配置解析]{Colors.ENDC}")

        # 查找 dpdk 配置段
        dpdk_elems = root.findall(".//dpdk") or root.findall(".//DPDK")
        if dpdk_elems:
            for dpdk in dpdk_elems:
                print(f"\n{Colors.CYAN}DPDK 配置段:{Colors.ENDC}")
                for child in dpdk:
                    tag = child.tag.split('}')[-1] if '}' in child.tag else child.tag
                    text = child.text.strip() if child.text else ""
                    print_item(tag, text, indent=1)

        # 查找 pci 设备配置
        pci_elems = root.findall(".//pci") or root.findall(".//PCI")
        if pci_elems:
            print(f"\n{Colors.CYAN}PCI 设备配置:{Colors.ENDC}")
            for pci in pci_elems:
                for child in pci:
                    tag = child.tag.split('}')[-1] if '}' in child.tag else child.tag
                    text = child.text.strip() if child.text else ""
                    print_item(tag, text, indent=1)

        # 查找 port 配置
        port_elems = root.findall(".//port") or root.findall(".//PORT")
        if port_elems:
            print(f"\n{Colors.CYAN}网络端口配置:{Colors.ENDC}")
            for i, port in enumerate(port_elems):
                print(f"  Port {i}:")
                for child in port:
                    tag = child.tag.split('}')[-1] if '}' in child.tag else child.tag
                    text = child.text.strip() if child.text else ""
                    print_item(tag, text, indent=2)

        # 查找 lcore 配置
        lcore_elems = root.findall(".//lcore") or root.findall(".//lcores")
        if lcore_elems:
            print(f"\n{Colors.CYAN}逻辑核心配置:{Colors.ENDC}")
            for lcore in lcore_elems:
                for child in lcore:
                    tag = child.tag.split('}')[-1] if '}' in child.tag else child.tag
                    text = child.text.strip() if child.text else ""
                    print_item(tag, text, indent=1)

        return config_data

    except ET.ParseError as e:
        print(f"{Colors.FAIL}XML 解析错误: {e}{Colors.ENDC}")
        return None
    except Exception as e:
        print(f"{Colors.FAIL}读取配置文件错误: {e}{Colors.ENDC}")
        return None


def execute_lazybsd_proxy():
    """执行 lazybsd_runner 可执行文件"""
    print_section("执行 lazybsd_runner")

    executable = "./lazybsd_runner"

    # 检查文件是否存在
    if not os.path.exists(executable):
        # 尝试查找其他可能的位置
        possible_paths = [
            "./lazybsd_runner",
            "lazybsd_runner",
            "../lazybsd_runner",
            "/usr/local/bin/lazybsd_runner",
            "/usr/bin/lazybsd_runner"
        ]

        found = False
        for path in possible_paths:
            if os.path.exists(path):
                executable = path
                found = True
                break

        if not found:
            print(f"{Colors.FAIL}错误: 找不到 lazybsd_runner 可执行文件{Colors.ENDC}")
            print(f"{Colors.WARNING}请确保 lazybsd_runner 在当前目录或系统 PATH 中{Colors.ENDC}")
            return False

    # 检查文件是否可执行
    if not os.access(executable, os.X_OK):
        print(f"{Colors.WARNING}警告: {executable} 可能没有执行权限，尝试添加...{Colors.ENDC}")
        try:
            os.chmod(executable, 0o755)
        except Exception as e:
            print(f"{Colors.FAIL}无法添加执行权限: {e}{Colors.ENDC}")
            return False

    print(f"{Colors.GREEN}找到可执行文件: {executable}{Colors.ENDC}")

    # 检查是否以 root 运行 (DPDK 通常需要 root 或特定权限)
    if os.geteuid() != 0:
        print(f"{Colors.WARNING}警告: 当前不是 root 用户，DPDK 应用通常需要 root 权限或 CAP_IPC_LOCK, CAP_NET_ADMIN 等能力{Colors.ENDC}")

    # 检查大页内存配置 (DPDK 需要)
    try:
        with open('/proc/meminfo', 'r') as f:
            meminfo = f.read()
            if 'HugePages_Total' in meminfo:
                match = re.search(r'HugePages_Total:\s+(\d+)', meminfo)
                if match:
                    hugepages = int(match.group(1))
                    if hugepages == 0:
                        print(f"{Colors.WARNING}警告: 系统未配置大页内存 (HugePages)，DPDK 可能无法正常运行{Colors.ENDC}")
                        print(f"  建议执行: echo 1024 > /proc/sys/vm/nr_hugepages")
                    else:
                        print(f"{Colors.GREEN}系统已配置大页内存: {hugepages} 页{Colors.ENDC}")
    except Exception as e:
        pass

    # 执行程序
    print(f"\n{Colors.BOLD}{Colors.GREEN}正在启动 lazybsd_runner...{Colors.ENDC}\n")

    try:
        # 使用 execv 替换当前进程，或 subprocess 创建新进程
        # 这里使用 subprocess 以便在 Python 中捕获输出
        env = os.environ.copy()

        # 可以在这里设置 DPDK 环境变量
        # env['RTE_SDK'] = '/path/to/dpdk'
        # env['RTE_TARGET'] = 'x86_64-native-linuxapp-gcc'

        result = subprocess.run(
            [executable],
            env=env,
            capture_output=False,  # 直接输出到终端
            text=True
        )

        if result.returncode != 0:
            print(f"\n{Colors.FAIL}lazybsd_runner 退出，返回码: {result.returncode}{Colors.ENDC}")
            return False

    except KeyboardInterrupt:
        print(f"\n{Colors.WARNING}用户中断执行{Colors.ENDC}")
    except Exception as e:
        print(f"{Colors.FAIL}执行失败: {e}{Colors.ENDC}")
        return False

    return True


def main():
    """主函数"""
    print(f"{Colors.BOLD}{Colors.HEADER}")
    print("=" * 60)
    print("       LazyBSD 启动脚本")
    print("       DPDK-based Userspace TCP/IP Stack")
    print("=" * 60)
    print(f"{Colors.ENDC}")

    # 步骤 1: 分析 CPU 和内存
    cpu_info, memory_info = analyze_cpu_and_memory()

    # 步骤 2: 解析配置文件
    config = parse_config_xml()

    # 步骤 3: 执行 lazybsd_runner
    print("\n")
    input(f"{Colors.WARNING}按 Enter 键启动 lazybsd_runner (Ctrl+C 取消)...{Colors.ENDC}")

    success = execute_lazybsd_proxy()

    if success:
        print(f"\n{Colors.GREEN}lazybsd_runner 执行完成{Colors.ENDC}")
    else:
        print(f"\n{Colors.FAIL}lazybsd_runner 执行失败{Colors.ENDC}")
        sys.exit(1)


if __name__ == "__main__":
    main()