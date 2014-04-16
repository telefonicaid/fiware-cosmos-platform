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
import unittest

from mock import MagicMock

from cosmos.cli.compute import add_compute_commands


class ComputeCommandsTest(unittest.TestCase):

    def test_add_compute_commands(self):
        parser = MagicMock()
        add_compute_commands(parser)
        added_commands = [call[1][0] for call in parser.add_parser.mock_calls
                          if call[0] == '']
        self.assertIn('list', added_commands)
        self.assertIn('list-services', added_commands)
        self.assertIn('show', added_commands)
        self.assertIn('create', added_commands)
        self.assertIn('terminate', added_commands)
