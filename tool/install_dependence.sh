#!/bin/sh

# 安装boost

install_boost()
{
    if [ ! -f boost_1_80_0.tar.gz ]; then
        curl https://boostorg.jfrog.io/artifactory/main/release/1.80.0/source/boost_1_80_0.tar.gz -o boost.tar.gz
    fi

    return 0
}

install_openssl()
{
    if [ ! -f openssl-3.0.5.tar.gz ]; then
        wget https://www.openssl.org/source/openssl-3.0.5.tar.gz
    fi

    return 0
}


time install_boost