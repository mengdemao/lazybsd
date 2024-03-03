#!/usr/bin/env python3

import pytest

from lazybsd import lazybsd

def test_version():
	print(lazybsd.version())
