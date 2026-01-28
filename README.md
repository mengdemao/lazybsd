# lazybsd

[![Build](https://github.com/mengdemao/lazybsd/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/mengdemao/lazybsd)
[![License](https://img.shields.io/github/license/mengdemao/lazybsd)](LICENSE)
[![Code Size](https://img.shields.io/github/languages/code-size/mengdemao/lazybsd)](https://github.com/mengdemao/lazybsd)
[![Releases](https://img.shields.io/github/downloads/mengdemao/lazybsd/total)](https://github.com/mengdemao/lazybsd/releases)
[![Coverage](https://codecov.io/gh/mengdemao/lazybsd/graph/badge.svg?token=GKH2X5GZJ8)](https://codecov.io/gh/mengdemao/lazybsd)

简体中文 | English

---

简介
----
lazybsd 是一个在 Linux/用户态中实现的 FreeBSD 风格网络协议栈，面向高性能网络应用场景。项目目标是为需要自定义或高性能协议栈的场景（如 NFV、负载均衡、网关、网络测试平台等）提供一个可移植且高效的实现，支持 DPDK 加速与标准 BSD Socket API 兼容层。

主要特性
------
- 纯用户态实现，无需修改内核
- 移植并兼容 FreeBSD 网络协议栈行为（IPv4/IPv6、TCP、UDP、ICMP 等）
- 提供与 BSD Socket 兼容的接口，降低迁移成本
- 可选集成 DPDK 实现高性能数据面（零拷贝、批量收发等优化）
- 支持多线程与事件驱动（epoll 兼容）
- 跨架构支持：x86_64 / aarch64（以构建与平台支持为准）
- 单元测试与代码覆盖（项目包含测试套件）

目录
----
- [快速开始](#快速开始)
  - [依赖](#依赖)
  - [构建与安装](#构建与安装)
- [使用示例](#使用示例)
- [运行测试](#运行测试)
- [文档](#文档)
- [贡献](#贡献)
- [常见问题](#常见问题)
- [许可](#许可)
- [致谢](#致谢)
- [维护者](#维护者)

快速开始
--------

### 依赖（以 Ubuntu/Debian 为例）
确保安装基础构建工具和可选依赖（根据你需要启用的功能调整）：

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake ninja-build pkg-config \
    libnuma-dev python3 python3-pip clang llvm graphviz doxygen
# DPDK（可选）：请参考 DPDK 官方安装说明
```

项目还使用若干 Python 脚本/工具：
```bash
pip3 install --user pyelftools
```

### 克隆仓库
```bash
git clone https://github.com/mengdemao/lazybsd.git
cd lazybsd
```

### 使用项目自带构建脚本（推荐）
项目包含 build.py 用于自动化构建与安装：

```bash
# 全量构建（包含可选步骤）
./build.py -a -c Release

# 安装到默认 ./install
./build.py -i
```

### 使用 CMake 构建
如果你更喜欢手动 CMake 构建：

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j$(nproc)
cmake --install . --prefix ../install
```

构建完成后，常见输出：
- 二进制： ./install/bin/
- 库文件： ./install/lib/
- 头文件： ./install/include/

使用示例
--------
下面示例展示如何在程序中使用 lazybsd 的兼容 socket 接口（示例为伪代码/演示）：

```c
#include <lazybsd.h>

int main(void) {
    if (lazybsd_init() != 0) {
        fprintf(stderr, "lazybsd init failed\n");
        return 1;
    }

    int sock = lazybsd_socket(AF_INET, SOCK_STREAM, 0);
    // 进行 bind/listen/accept/connect 等操作（行为兼容 BSD socket）
    lazybsd_close(sock);

    lazybsd_cleanup();
    return 0;
}
```

详尽示例见仓库 `examples/` 目录（例如 http_server、client 等）。

运行测试
--------
项目包含单元测试与集成测试示例：

```bash
# 假设在项目根目录并已构建
./build/test/lazybsd_test
# 或运行示例
./build/examples/http_server
```

若要生成文档（本地）：
```bash
sudo apt install doxygen graphviz
doxygen docs/Doxyfile
# 打开 docs/html/index.html
```

文档
----
- 在线 API 文档与说明： https://mengdemao.github.io/lazybsd/
- 源代码中的注释与 docs/ 目录包含设计文档与实现细节

贡献
----
欢迎贡献！请先阅读 CONTRIBUTING.md 并遵循以下基本流程：

1. Fork 本仓库
2. 创建分支： `git checkout -b feature/your-feature`
3. 提交修改并推送： `git commit -am "Describe changes"` / `git push origin feature/your-feature`
4. 提交 Pull Request（PR）并在 PR 中描述测试步骤与变更影响

代码风格、测试覆盖与 CI 校验在贡献时均会被审查。若需提出设计讨论，请先在 Issues 中发起议题。

常见问题 / Troubleshooting
--------------------------
Q: 我该如何开启 DPDK 支持？
A: 请先在目标机器上安装并配置 DPDK，确保内核驱动与绑定正确。然后在构建时启用相应 CMake/构建选项（或通过 build.py 的参数）。

Q: lazybsd 是否会影响系统内核网络？
A: lazybsd 在用户态运行，默认不会修改内核网络栈配置，但在使用 DPDK 等零拷贝设备绑定时，可能需要将网卡从内核驱动解绑。

Q: 我在编译时遇到未找到头文件/库的问题
A: 请确认已安装所需依赖（libnuma、clang/llvm、python3-dev 等），并检查 CMake 输出的错误日志。

许可
----
本项目采用 BSD 3-Clause License，详见 [LICENSE](LICENSE)。

致谢
----
- FreeBSD 项目 — 提供网络协议栈设计思路与实现参考
- DPDK 社区 — 高性能数据面方案
- LLVM / Clang — 编译器与工具链支持

维护者
------
- mengdemao (https://github.com/mengdemao)

联系方式
--------
如需联系或报告安全问题请在仓库 Issues 中创建 issue，或在 README 页面/项目主页查找维护者信息。

版本与变更
----------
请查看仓库 Releases 或 CHANGELOG（如果存在）获取版本历史与重要变更说明。

---

感谢使用 lazybsd！