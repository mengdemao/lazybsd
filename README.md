# lazybsd

![Github Action](https://github.com/mengdemao/lazybsd/actions/workflows/build.yml/badge.svg?branch=master)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/mengdemao/lazybsd)
![GitHub all releases](https://img.shields.io/github/downloads/mengdemao/lazybsd/total)
![GitHub](https://img.shields.io/github/license/mengdemao/lazybsd)
![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/mengdemao/lazybsd)
![GitHub commit activity](https://img.shields.io/github/commit-activity/y/mengdemao/lazybsd)
[![CodeFactor](https://www.codefactor.io/repository/github/mengdemao/lazybsd/badge)](https://www.codefactor.io/repository/github/mengdemao/lazybsd)
[![codecov](https://codecov.io/gh/mengdemao/lazybsd/graph/badge.svg?token=GKH2X5GZJ8)](https://codecov.io/gh/mengdemao/lazybsd)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/29112/badge.svg)](https://scan.coverity.com/projects/mengdemao-lazybsd)

FreeBSD 网络协议栈的 Linux 用户层移植实现

## 目录

- [项目背景](#项目背景)
- [主要特性](#主要特性)
- [快速开始](#快速开始)
  - [依赖安装](#依赖安装)
  - [编译安装](#编译安装)
- [使用指南](#使用指南)
- [API 文档](#api-文档)
- [开发贡献](#开发贡献)
- [许可证](#许可证)
- [致谢](#致谢)

## 项目背景

lazybsd 项目旨在将 FreeBSD 的高性能网络协议栈移植到 Linux 用户空间，为需要定制网络协议栈或进行网络协议开发的场景提供灵活的基础设施。项目特点包括：

- 纯用户态实现，无需修改内核
- 支持主流 CPU 架构（x86_64/AArch64）
- 兼容 Linux 现有网络工具链
- 提供标准 BSD Socket API

## 主要特性

✅ 完整移植 FreeBSD 13 网络协议栈 </br>
✅ 支持 TCP/UDP/ICMP 等核心协议 </br>
✅ 提供 epoll 兼容的事件驱动接口 </br>
✅ 集成 DPDK 高性能数据面支持 </br>
✅ 多线程安全架构设计 </br>
✅ 完善的单元测试覆盖（覆盖率 >85%）</br>

## 快速开始

### 依赖安装

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    meson ninja-build \
    libnuma-dev \
    pkg-config \
    llvm-15 clang-15 \
    python3-pip

# 安装编译工具链
pip3 install --user pyelftools
```

### 编译安装

```bash
git clone https://github.com/mengdemao/lazybsd.git
cd lazybsd

# 配置编译选项
meson setup build \
    -Dbuildtype=release \
    -Doptimization=3 \
    -Db_pie=true \
    -Dwarning_level=3

# 编译并安装
ninja -C build
sudo ninja -C build install
```

[![编译演示](https://asciinema.org/a/YmrjuKG5R3Hc97fb3e8xxZsX4.svg)](https://asciinema.org/a/YmrjuKG5R3Hc97fb3e8xxZsX4)

## 使用指南

### 基础示例

```c
#include <lazybsd.h>

int main() {
    lazybsd_init();

    int sock = lazybsd_socket(AF_INET, SOCK_STREAM, 0);
    // ... BSD socket 标准用法 ...

    lazybsd_cleanup();
    return 0;
}
```

### 运行测试用例

```bash
# 运行单元测试
./build/test/lazybsd_test

# 启动示例 HTTP 服务器
./build/examples/http_server
```

## API 文档

项目提供完整的 API 文档，可通过以下方式访问：

- 在线文档：https://mengdemao.github.io/lazybsd/
- 本地构建文档：
  ```bash
  sudo apt install doxygen graphviz
  doxygen docs/Doxyfile
  xdg-open docs/html/index.html
  ```

## 开发贡献

我们欢迎各种形式的贡献！请先阅读[贡献指南](CONTRIBUTING.md)，以下是基本流程：

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/your-feature`)
3. 提交修改 (`git commit -am 'Add some feature'`)
4. 推送到远程分支 (`git push origin feature/your-feature`)
5. 创建 Pull Request

## 许可证

本项目采用 [BSD 3-Clause License](LICENSE)

## 致谢

- FreeBSD 项目 - 提供优秀的网络协议栈实现
- DPDK 社区 - 高性能数据面支持
- LLVM 项目 - 现代编译工具链
