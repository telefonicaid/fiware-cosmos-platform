# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
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
