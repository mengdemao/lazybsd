#!/bin/bash

ROOT_PATH=$(pwd)

[ -d build ] && rm -rf build

echo "安装..."
conan install conanfile.txt --build=missing -s build_type=Debug

cmake --preset conan-debug

echo "配置..."
cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_TOOLCHAIN_FILE="${ROOT_PATH}"/build/Debug/generators/conan_toolchain.cmake \
      -S "${ROOT_PATH}" \
      -B "${ROOT_PATH}"/build \
      -G Ninja

echo "构建..."
cmake --build build --config  Debug -j"$(nproc)"

echo "运行..."
ctest -C Debug