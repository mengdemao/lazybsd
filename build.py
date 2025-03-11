#!/usr/bin/env python3
"""
lazybsd build system

Features:
- Manage Conan dependencies
- Generate CMake build system
- Compile project code
- Run unit tests
- Setup DPDK environment

Authors: lazybsd team
Version: 1.2.0
Last Updated: 2025-03-12
"""
import os
import sys
import subprocess
import argparse
from datetime import datetime
from typing import Optional

# Initialize core environment variables
ROOT_PATH = os.getcwd()  # Project root directory
BUILD_PATH = os.path.join(ROOT_PATH, "build")
INSTALL_PATH = os.path.join(ROOT_PATH, "install")
os.environ["LD_LIBRARY_PATH"] = f"{INSTALL_PATH}/lib:{os.environ.get('LD_LIBRARY_PATH', '')}"

class Colors:
    """Terminal color control codes"""
    HEADER = '\033[95m'
    INFO = '\033[92m'
    WARN = '\033[93m'
    ERROR = '\033[91m'
    ENDC = '\033[0m'

def log_err(message: str):
    """Log error message
    Args:
        message (str): Error description
    """
    log_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    caller = sys._getframe(1).f_code.co_name
    print(f"{Colors.ERROR}[ERROR][{log_time}][{caller}] {message}{Colors.ENDC}")

def log_warn(message: str):
    """Log warning message
    Args:
        message (str): Warning description
    """
    log_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    caller = sys._getframe(1).f_code.co_name
    print(f"{Colors.WARN}[WARN][{log_time}][{caller}] {message}{Colors.ENDC}")

def log_info(message: str):
    """Log informational message
    Args:
        message (str): Information content
    """
    log_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    caller = sys._getframe(1).f_code.co_name
    print(f"{Colors.INFO}[INFO][{log_time}][{caller}] {message}{Colors.ENDC}")

def check_config(build_cfg: str) -> bool:
    """Validate build configuration
    Args:
        build_cfg (str): Build configuration (Debug/Release)
    Returns:
        bool: True if valid, False otherwise
    """
    if build_cfg not in ("Debug", "Release"):
        log_err("Invalid configuration, must be Debug or Release")
        return False
    return True

def run_command(cmd: str, cwd: Optional[str] = None) -> bool:
    """Execute shell command
    Args:
        cmd (str): Command to execute
        cwd (Optional[str]): Working directory
    Returns:
        bool: True if successful, False on error
    Raises:
        subprocess.CalledProcessError: On command failure
    """
    try:
        # Execute command with safety checks and output capture
        result = subprocess.run(
            cmd,
            shell=True,       # Allow shell features
            check=True,       # Throw exception on failure
            cwd=cwd,          # Working directory
            stdout=subprocess.PIPE,  # Capture stdout
            stderr=subprocess.STDOUT, # Combine stderr into stdout
            text=True         # Return string output instead of bytes
        )
        if result.stdout:
            print(result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        log_err(f"Command failed: {e.cmd}\n{e.stdout}")
        return False

def conan_config(build_cfg: str):
    """Configure Conan dependencies
    1. Validate build configuration
    2. Install dependencies for both Debug/Release
    3. Generate toolchain files

    Args:
        build_cfg (str): Current build configuration
    """
    if not check_config(build_cfg):
        sys.exit(1)

    log_info("Configuring Conan")
    # Install dependencies for both configurations to allow build type switching
            f"conan install conanfile.txt --build=missing "
            f"-s build_type={cfg} -s compiler.cppstd=gnu23"
        )
        if not run_command(cmd):
            sys.exit(1)

def cmake_preset():
    for preset in ["conan-debug", "conan-release"]:
        cmd = f"cmake --preset {preset}"
        if not run_command(cmd):
            sys.exit(1)

def cmake_config(build_cfg: str, build_cov: bool):
    """Configure CMake build system
    1. Set build type
    2. Configure code coverage
    3. Specify Conan toolchain
    4. Generate Ninja files

    Args:
        build_cfg (str): Build type (Debug/Release)
        build_cov (bool): Enable code coverage
    """
    if not check_config(build_cfg):
        sys.exit(1)

    log_info(f"CMake configure - Build: {build_cfg}, Coverage: {build_cov}")
    cmd = (
        f"cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW "
        f"-DCMAKE_BUILD_TYPE={build_cfg} "
        f"-DBUILD_COVERAGE={'ON' if build_cov else 'OFF'} "
        f"-DCMAKE_TOOLCHAIN_FILE={os.path.join(ROOT_PATH, 'build', build_cfg, 'generators', 'conan_toolchain.cmake')} "
        f"-S {ROOT_PATH} -B {BUILD_PATH} -G Ninja"
    )
    if not run_command(cmd):
        sys.exit(1)

def cmake_build():
    cmd = f"cmake --build {BUILD_PATH} -j{os.cpu_count()}"
    if not run_command(cmd):
        sys.exit(1)

def cmake_ctest(build_cfg: str):
    if not check_config(build_cfg):
        sys.exit(1)

    cmd = f"ctest -C {build_cfg} --test-dir {os.path.join(BUILD_PATH, 'test')}"
    if not run_command(cmd):
        sys.exit(1)

def setup_dpdk():
    """Install and configure DPDK
    Steps:
    1. Change to DPDK source directory
    2. Configure with meson
    3. Build with ninja
    4. Install dependencies
    5. Return to root directory
    """
    log_info("Building DPDK")
    dpdk_dir = os.path.join(ROOT_PATH, "dpdk")

    try:
        os.chdir(dpdk_dir)
        if not os.path.exists("build"):
            os.makedirs("build", exist_ok=True)
            meson_cmd = (
                "meson setup --prefix={} -Dbuildtype=debug "
                "-Denable_kmods=true -Dexamples=all -Dplatform=native build"
            ).format(INSTALL_PATH)
            if not run_command(meson_cmd):
                raise Exception("Meson setup failed")

            if not run_command("ninja -C build"):
                raise Exception("Ninja build failed")

        if not os.path.exists(INSTALL_PATH):
            if not run_command("ninja -C build install"):
                raise Exception("Ninja install failed")

        log_info("DPDK build completed")
    except Exception as e:
        log_err(f"DPDK build failed: {str(e)}")
        sys.exit(1)
    finally:
        os.chdir(ROOT_PATH)

def build_all(build_cfg: str, build_cov: bool):
    if not check_config(build_cfg):
        sys.exit(1)

    log_info("Cleaning build directory")
    if os.path.exists(BUILD_PATH):
        import shutil
        shutil.rmtree(BUILD_PATH)
    os.makedirs(BUILD_PATH, exist_ok=True)

    log_info("Starting full build")
    try:
        conan_config(build_cfg)
        cmake_preset()
        cmake_config(build_cfg, build_cov)
        cmake_build()
        cmake_ctest(build_cfg)
    except Exception as e:
        log_err(f"Build failed: {str(e)}")
        sys.exit(1)

def main():
    """Command line entry point
    Options:
    -c/--config : Build configuration (Debug/Release)
    -a/--all    : Full build process
    -b/--build  : Build step only
    -l/--coverage: Enable code coverage
    -s/--setup  : Install DPDK

    Examples:
    ./build.py -a -c Release  # Full release build
    ./build.py -s             # Install DPDK only
    """
    parser = argparse.ArgumentParser(description="Build system for lazybsd")
    parser.add_argument("-c", "--config", choices=["Debug", "Release"], default="Debug",
                        help="Build configuration (Debug/Release)")
    parser.add_argument("-a", "--all", action="store_true",
                        help="Perform full build process")
    parser.add_argument("-b", "--build", action="store_true",
                        help="Run build step only")
    parser.add_argument("-l", "--coverage", action="store_true",
                        help="Enable code coverage")
    parser.add_argument("-s", "--setup", action="store_true",
                        help="Setup DPDK dependencies")

    args = parser.parse_args()

    if not check_config(args.config):
        sys.exit(1)

    try:
        if args.setup:
            setup_dpdk()
            return

        if args.build:
            cmake_build()
            return

        if args.all:
            build_all(args.config, args.coverage)
            return

    except KeyboardInterrupt:
        log_info("Build process interrupted by user")
        sys.exit(1)

if __name__ == "__main__":
    main()
