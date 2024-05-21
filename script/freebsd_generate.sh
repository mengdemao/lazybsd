#!/bin/bash

BSD_DIR=$1

if [ ! -d include ]
then
	mkdir include
	pushd include >> /dev/null || exit

	awk -f "${BSD_DIR}"/sys/tools/makeobjops.awk "${BSD_DIR}"/sys/kern/bus_if.m -h
	awk -f "${BSD_DIR}"/sys/tools/makeobjops.awk "${BSD_DIR}"/sys/kern/device_if.m -h
	awk -f "${BSD_DIR}"/sys/tools/makeobjops.awk "${BSD_DIR}"/sys/opencrypto/cryptodev_if.m -h
	awk -f "${BSD_DIR}"/sys/tools/makeobjops.awk "${BSD_DIR}"/sys/opencrypto/cryptodev_if.m -c
	awk -f "${BSD_DIR}"/sys/tools/makeobjops.awk "${BSD_DIR}"/sys/kern/linker_if.m -h
	awk -f "${BSD_DIR}"/sys/tools/makeobjops.awk "${BSD_DIR}"/sys/kern/linker_if.m -c
	awk -f "${BSD_DIR}"/sys/tools/vnode_if.awk   "${BSD_DIR}"/sys/kern/vnode_if.src -p
	awk -f "${BSD_DIR}"/sys/tools/vnode_if.awk   "${BSD_DIR}"/sys/kern/vnode_if.src -q
	awk -f "${BSD_DIR}"/sys/tools/vnode_if.awk   "${BSD_DIR}"/sys/kern/vnode_if.src -h

	mkdir -p machine
	cp -rfv "${BSD_DIR}"/sys/amd64/include/* machine

	mkdir -p x86
	cp -rfv "${BSD_DIR}"/sys/x86/include/* x86

	popd >> /dev/null || exit
fi