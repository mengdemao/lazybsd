# lazybsd

![Github Action](https://github.com/mengdemao/lazybsd/actions/workflows/build.yml/badge.svg?branch=master)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/mengdemao/lazybsd)
![GitHub all releases](https://img.shields.io/github/downloads/mengdemao/lazybsd/total)
![GitHub](https://img.shields.io/github/license/mengdemao/lazybsd)
![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/mengdemao/lazybsd)
![GitHub commit activity](https://img.shields.io/github/commit-activity/y/mengdemao/lazybsd)

freebsd网络协议栈Linux用户层移植

## 目录

- [lazybsd](#lazybsd)
	- [目录](#目录)
	- [背景](#背景)
		- [编译freebsd](#编译freebsd)
	- [安装](#安装)
	- [使用](#使用)
	- [API](#api)
	- [贡献](#贡献)
	- [许可证](#许可证)

## 背景

### 编译freebsd

```shell
$ export MAKEOBJDIRPREFIX=${PWD%/*}/build

$ ./tools/build/make.py --debug --cross-bindir=/usr/lib/llvm-15/bin TARGET=amd64 TARGET_ARCH=amd64 -n
$ ./tools/build/make.py --debug --cross-bindir=/usr/lib/llvm-15/bin TARGET=amd64 TARGET_ARCH=amd64 kernel-toolchain -s -j4
$ ./tools/build/make.py --debug --cross-bindir=/usr/lib/llvm-15/bin TARGET=amd64 TARGET_ARCH=amd64 KERNCONF=GENERIC NO_MODULES=yes buildkernel -s -j4

$ ./tools/build/make.py --debug --cross-bindir=/usr/lib/llvm-15/bin TARGET=arm64 TARGET_ARCH=aarch64 -n
$ ./tools/build/make.py --debug --cross-bindir=/usr/lib/llvm-15/bin TARGET=arm64 TARGET_ARCH=aarch64 kernel-toolchain -s -j4
$ ./tools/build/make.py --debug --cross-bindir=/usr/lib/llvm-15/bin TARGET=arm64 TARGET_ARCH=aarch64 KERNCONF=GENERIC NO_MODULES=yes buildkernel -s -j4
```

## 安装

```shell
# 安装软件
sudo apt install doxygen graphviz python3 python3-pip libgtest-dev ninja-build

# 安装conan
pip3 install conan
export PATH=~/.local/bin/:$PATH

conan profile detect --force
conan install . --output-folder=build --build=missing

# 配置编译
cmake -B build -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

# 执行编译
cmake --build build --config  Release/Debug/RelWithDebInfo

# 执行测试
ctest -C Release/Debug/RelWithDebInfo
```

## 使用

## API

## 贡献

See [the contributing file](CONTRIBUTING.md)!

## 许可证

[BSD 3-Clause License](LICENSE)