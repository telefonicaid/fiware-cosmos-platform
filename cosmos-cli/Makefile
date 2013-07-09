# -*- coding: utf-8 -*-
#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
#

PYTHON?=python

all:
	$(PYTHON) ./bootstrap.py && bin/buildout -c dev.cfg

clean:
	rm -rf parts eggs bin develop-eggs coverage build dist lib include \
	       cosmos.egg-info .Python .installed.cfg

.PHONY: all clean
