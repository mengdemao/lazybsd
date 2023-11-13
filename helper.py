#!/usr/bin/env python3

import os
import argparse
import subprocess
from rich.console import Console

def pwd():
    '''
    打印当前路径名
    '''
    return os.getcwd()

def chdir(new_dir):
    '''
    改变当前路径名
    '''
    return os.chdir(new_dir)

def shell(cmd):
    '''
    执行命令
    '''
    subprocess.run(cmd, shell=True)

def system(cmd):
    '''
    执行立即命令
    '''
    os.system(cmd)

def log_error(txt):
    '''
    错误日志
    '''
    console = Console()
    console.log(txt)

def log_warning(txt):
    '''
    报警日志
    '''
    console = Console()
    console.log(txt)

def log_debug(txt):
    '''
    调试日志
    '''
    console = Console()
    console.log(txt)

def log_print(txt):
    '''
    打印日志
    '''
    console = Console()
    console.print(txt)

def log_help():
    '''
    打印帮助文档
    '''
    log_print("LazyBsd helper")

def build(Debug=True):
    '''
    执行构建
    '''
    if Debug == True:
        shell("./build.sh --config=Debug")
    else:
        shell("./build.sh --config=Debug")


def run():
    '''
    运行程序
    '''

def test():
    '''
    测试程序
    '''

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--version', '-v', action='version',
                    version='%(prog)s version : v 0.01', help='show the version')
    parser.add_argument('--debug', '-d', action='store_true',
                    help='show the version',
                    default=False)
    args = parser.parse_args()
