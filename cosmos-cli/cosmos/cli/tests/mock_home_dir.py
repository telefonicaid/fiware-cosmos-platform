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
"""Utility for replacing filesystem config files with in-memory mocks"""

from mock import MagicMock, patch
import yaml

from cosmos.cli.home_dir import HomeDir


class mock_home(object):
    """Context in which the module cosmos.cli.home_dir.get_home_dir returns
    a configurable mocked object."""

    def __init__(self, **kwargs):
        self.home_dir = MockHomeDir(**kwargs)
        self.patch = patch('cosmos.cli.home_dir.get',
                           MagicMock(return_value=self.home_dir))

    def __enter__(self):
        self.patch.__enter__()
        return self.home_dir

    def __exit__(self, *args):
        return self.patch.__exit__(*args)


class MockHomeDir(HomeDir):

    def __init__(self, config=None, last_cluster=None):
        if isinstance(config, dict):
            self.config = yaml.dump(config, default_flow_style=False)
        else:
            self.config = config
        self.last_cluster = last_cluster

    def read_last_cluster(self):
        return self.last_cluster

    def write_last_cluster(self, contents):
        self.last_cluster = contents

    def read_config_file(self, filename_override=None):
        if filename_override is not None:
            raise NotImplementedError()
        return self.config

    def write_config_file(self, contents):
        self.config = contents
