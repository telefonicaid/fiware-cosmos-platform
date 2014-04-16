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

from cosmos.cli.terminal import get_terminal_size, TerminalError

class TerminalTest(unittest.TestCase):

    def test_get_terminal_size(self):
        try:
            term_size = get_terminal_size()
            self.assertIsInstance(term_size, tuple)
            self.assertEqual(2, len(term_size))
            self.assertIsInstance(term_size[0], int)
            self.assertIsInstance(term_size[1], int)
        except TerminalError:
            pass
