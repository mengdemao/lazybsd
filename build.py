#!/usr/bin/env python3
"""
lazybsd build system

Features:
- Generate CMake build system
- Compile project code
- Run unit tests
- Setup DPDK environment

Authors: lazybsd team
Version: 1.3.0
Last Updated: 2025-09-13
"""
import os
import sys
import subprocess
import argparse
import inspect
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
    caller = inspect.stack()[1].function
    print(
        f"{Colors.ERROR}[ERROR][{log_time}][{caller}] {message}{Colors.ENDC}")


def log_warn(message: str):
    """Log warning message
    Args:
        message (str): Warning description
    """
    log_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    caller = inspect.stack()[1].function
    print(f"{Colors.WARN}[WARN][{log_time}][{caller}] {message}{Colors.ENDC}")


def log_info(message: str):
    """Log informational message
    Args:
        message (str): Information content
    """
    log_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    caller = inspect.stack()[1].function
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
            stderr=subprocess.STDOUT,  # Combine stderr into stdout
            text=True         # Return string output instead of bytes
        )
        if result.stdout:
            print(result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        log_err(f"Command failed: {e.cmd}\n{e.stdout}")
        return False


def cmake_config(build_cfg: str, build_cov: bool, use_mold: bool = False):
    """Configure CMake build system
    1. Set build type
    2. Configure code coverage
    3. Configure mold linker
    4. Generate Ninja files

    Args:
        build_cfg (str): Build type (Debug/Release)
        build_cov (bool): Enable code coverage
        use_mold (bool): Use mold linker
    """
    if not check_config(build_cfg):
        sys.exit(1)

    log_info(
        f"CMake configure - Build: {build_cfg}, Coverage: {build_cov}, Mold: {use_mold}")
    cmd = (
        f"cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW "
        f"-DCMAKE_BUILD_TYPE={build_cfg} "
        f"-DBUILD_COVERAGE={'ON' if build_cov else 'OFF'} "
        f"-DUSE_MOLD_LINKER={'ON' if use_mold else 'OFF'} "
        f"-S {ROOT_PATH} -B {BUILD_PATH} -G Ninja"
    )
    if not run_command(cmd):
        sys.exit(1)


def cmake_build():
    """Build project using CMake build command
    Utilizes all CPU cores for parallel compilation
    1. Execute build command
    2. Handle build errors
    3. Output build logs
    """
    cmd = f"cmake --build {BUILD_PATH} -j{os.cpu_count()}"
    if not run_command(cmd):
        sys.exit(1)


def cmake_ctest(build_cfg: str):
    """Run unit tests using CTest
    1. Validate build configuration
    2. Execute tests in build directory
    3. Handle test failures

    Args:
        build_cfg (str): _description_
    """
    if not check_config(build_cfg):
        sys.exit(1)

    cmd = f"ctest -C {build_cfg} --test-dir {os.path.join(BUILD_PATH, 'test')}"
    if not run_command(cmd):
        sys.exit(1)


def cmake_install():
    """Install built artifacts using CMake install command
    Installs libraries, executables, and headers to INSTALL_PATH
    """
    log_info(f"Installing to {INSTALL_PATH}")
    cmd = f"cmake --install {BUILD_PATH} --prefix {INSTALL_PATH}"
    if not run_command(cmd):
        sys.exit(1)
    log_info("Installation completed successfully")


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
                "meson setup   -Ddisable_drivers=* "
                "-Denable_drivers=net/e1000e,common/iavf,net/pcap,net/af_xdp net/bonding net/kni bus/vmbus "
                "--prefix={} -Dbuildtype=release "
                "-Dexamples= -Dplatform=native build"
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


def build_all(build_cfg: str, build_cov: bool, use_mold: bool = False):
    """Perform full build process
    Steps:
    1. Validate configuration
    2. Clean build directory
    3. Configure CMake
    4. Build project
    5. Run unit tests

    Args:
        build_cfg (str): Build configuration (Debug/Release)
        build_cov (bool): Enable code coverage
        use_mold (bool): Use mold linker
    """
    if not check_config(build_cfg):
        sys.exit(1)

    log_info("Cleaning build directory")
    if os.path.exists(BUILD_PATH):
        import shutil
        shutil.rmtree(BUILD_PATH)
    os.makedirs(BUILD_PATH, exist_ok=True)

    log_info("Starting full build")
    try:
        cmake_config(build_cfg, build_cov, use_mold)
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
    -i/--install: Install built artifacts to local directory

    Examples:
    ./build.py -a -c Release  # Full release build
    ./build.py -a -c Release -m  # Full release build with mold linker
    ./build.py -s             # Install DPDK only
    ./build.py -i             # Install built artifacts
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
    parser.add_argument("-m", "--mold", action="store_true",
                        help="Use mold linker")
    parser.add_argument("-s", "--setup", action="store_true",
                        help="Setup DPDK dependencies")
    parser.add_argument("-i", "--install", action="store_true",
                        help="Install built artifacts to local directory")

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

        if args.install:
            cmake_install()
            return

        if args.all:
            build_all(args.config, args.coverage, args.mold)
            return

    except KeyboardInterrupt:
        log_info("Build process interrupted by user")
        sys.exit(1)


if __name__ == "__main__":
    main()
