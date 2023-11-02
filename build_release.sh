#!/bin/bash

ROOT_PATH=$(pwd)

[ -d build ] && rm -rf build

echo "安装..."
conan install conanfile.txt --build=missing -s build_type=Release

cmake --preset conan-release

echo "配置..."
cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_TOOLCHAIN_FILE="${ROOT_PATH}"/build/Release/generators/conan_toolchain.cmake \
      -S "${ROOT_PATH}" \
      -B "${ROOT_PATH}"/build \
      -G Ninja

echo "构建..."
cmake --build build --config  Release -j"$(nproc)"

echo "运行..."
ctest -C Release