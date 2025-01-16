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

freebsd网络协议栈Linux用户层移植

## 目录

- [lazybsd](#lazybsd)
	- [目录](#目录)
	- [背景](#背景)
		- [编译freebsd](#编译freebsd)
		- [编译依赖](#编译依赖)
	- [编译安装](#编译安装)
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

### 编译依赖

```shell
$ sudo apt-get install build-essential
$ sudo apt install meson ninja-build
$ sudo apt install libnuma-dev

$ mkdir build
$ meson build
$ ninja -C build
```

## 编译安装

[![asciicast](https://asciinema.org/a/YmrjuKG5R3Hc97fb3e8xxZsX4.svg)](https://asciinema.org/a/YmrjuKG5R3Hc97fb3e8xxZsX4)

## 使用

## API

## 贡献

See [the contributing file](CONTRIBUTING.md)!

## 许可证

[BSD 3-Clause License](LICENSE)