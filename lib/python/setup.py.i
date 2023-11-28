import sys
from setuptools import setup, find_packages

try:
    from wheel.bdist_wheel import bdist_wheel as _bdist_wheel

    class bdist_wheel(_bdist_wheel):
        def finalize_options(self):
            _bdist_wheel.finalize_options(self)
            self.root_is_pure = False


except ImportError:
    bdist_wheel = None

if sys.version_info < (3, 11):
    sys.exit("Sorry, Python < 3.11 is not supported")

requirements = ["requests"]

setup(
    name="lazybsd",
    version="${PACKAGE_VERSION}",
    author="mengdemao",
    author_email="mengdemao19951021@163.com",
    maintainer="mengdemao",
    maintainer_email="mengdemao19951021@163.com",
    description="lazybsd is a high-performance TCP/IP network framework",
    url="https://github.com/mengdemao/lazybsd",
    classifiers=[
        "Programming Language :: C++",
        "Programming Language :: Python :: 3.11",
        "License :: OSI Approved :: BSD License",
        "Operating System :: OS Independent",
    ],
    license="BSD-3",
    python_requires=">=3.11",
    packages=find_packages(),
    package_dir={"": "."},
    package_data={"lazybsd": ["lazybsd${PYTHON_MODULE_PREFIX}${PYTHON_MODULE_EXTENSION}"]},
    install_requires=requirements,
    cmdclass={"bdist_wheel": bdist_wheel},
)
