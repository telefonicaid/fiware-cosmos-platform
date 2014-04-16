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

from testfixtures import TempDirectory

import cosmos.cli.main as main
from cosmos.common.tests.util import collect_outputs


class MainTest(unittest.TestCase):

    def setUp(self):
        self.parser = main.build_argument_parser()

    def test_exit_on_wrong_arguments(self):
        with collect_outputs() as outputs:
            self.assertRaises(SystemExit, self.parser.parse_args,
                              "wrong arguments".split())
            self.assertIn("error: invalid choice", outputs.stderr.getvalue())

    def test_parse_correct_arguments(self):
        self.assertValidArguments("configure")
        self.assertValidArguments("list")
        self.assertValidArguments("info")
        self.assertValidArguments("show 12345678901234567890123456789012")
        self.assertValidArguments("terminate 12345678901234567890123456789012")
        self.assertValidArguments("create --name foo --size 3")
        self.assertValidArguments("create --name foo --size 3 --shared")
        self.assertValidArguments("create --name foo --size 3 --services FOO BAR")
        self.assertValidArguments("create --name foo --size 3 --services FOO BAR --shared")
        self.assertValidArguments("ssh 12345678901234567890123456789012")
        self.assertValidArguments("ls /tmp")
        self.assertValidArguments("rm /tmp")
        self.assertValidArguments("get /tmp/file.csv ../Downloads/")
        with TempDirectory() as tmp:
            path = tmp.write('local.txt', 'content')
            self.assertValidArguments("put %s /remote/path/" % path)

    def assertValidArguments(self, arguments):
        self.assertIsNotNone(self.parser.parse_args(arguments.split()))
