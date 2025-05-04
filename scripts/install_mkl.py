#!/usr/bin/env python3

import os
import venv
from pathlib import Path

BUILD_DIR = Path('build')
VENV_DIR = BUILD_DIR / 'venv'

if not BUILD_DIR.exists():
	BUILD_DIR.mkdir()

env_builder = venv.EnvBuilder(
	system_site_packages=False,
	clear=False,
	symlinks=False,
	upgrade=False,
	with_pip=True,
	prompt=None,
)

env_builder.create(VENV_DIR)

os.chdir(BUILD_DIR)
os.system("venv\\Scripts\\activate && pip install mkl-static")
