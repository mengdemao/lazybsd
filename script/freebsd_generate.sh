#!/bin/bash

SRC_DIR=$1

if [ ! -d include ]
then
	mkdir include
	pushd include >> /dev/null

	awk -f ${SRC_DIR}/src/sys/tools/makeobjops.awk ${SRC_DIR}/src/sys/kern/bus_if.m -h
	awk -f ${SRC_DIR}/src/sys/tools/makeobjops.awk ${SRC_DIR}/src/sys/kern/device_if.m -h
	awk -f ${SRC_DIR}/src/sys/tools/makeobjops.awk ${SRC_DIR}/src/sys/opencrypto/cryptodev_if.m -h
	awk -f ${SRC_DIR}/src/sys/tools/makeobjops.awk ${SRC_DIR}/src/sys/opencrypto/cryptodev_if.m -c
	awk -f ${SRC_DIR}/src/sys/tools/makeobjops.awk ${SRC_DIR}/src/sys/kern/linker_if.m -h
	awk -f ${SRC_DIR}/src/sys/tools/makeobjops.awk ${SRC_DIR}/src/sys/kern/linker_if.m -c
	awk -f ${SRC_DIR}/src/sys/tools/vnode_if.awk   ${SRC_DIR}/src/sys/kern/vnode_if.src -p
	awk -f ${SRC_DIR}/src/sys/tools/vnode_if.awk   ${SRC_DIR}/src/sys/kern/vnode_if.src -q
	awk -f ${SRC_DIR}/src/sys/tools/vnode_if.awk   ${SRC_DIR}/src/sys/kern/vnode_if.src -h

	mkdir -p machine
	cp -rfv ${SRC_DIR}/src/sys/amd64/include/* machine

	mkdir -p x86
	cp -rfv ${SRC_DIR}/src/sys/x86/include/* x86

	popd >> /dev/null
fi