#!/bin/bash

# shellcheck disable=SC2086

# 获取工程的根目录
PATH_ROOT=$(git rev-parse --show-toplevel)

# 设置源地目录
PATH_SRC=$1
PATH_DST=${PATH_ROOT}/freebsd

check_dst() {
    pushd ${PATH_SRC} >> /dev/null || return 1
    echo "检查FreeBSD系统文件"

    # 检查FreeBSD是否是git仓库
    if [ ! -d .git ]; then
        echo "FreeBSD不存在git仓库"
        return 1
    fi

    echo "检查FreeBSD系统结束"
    popd >> /dev/null || return 1 
    return 0
}



###################################################################
#                                                                 #
#                           脚本运行起点                           #                                                
#                                                                 #
###################################################################

echo "拷贝文件开始"

# 创建目录
# [ -d ${PATH_DST} ] && rm -rf ${PATH_DST}
# mkdir -p ${PATH_DST}

# 检查freebsd文件夹是否有效
check_dst || exit 1

# 拷贝sys文件
# cp -rfv ${PATH_SRC}/sys ${PATH_DST}

# 拷贝头文件
mkdir -p ${PATH_DST}/include

echo "拷贝文件结束"