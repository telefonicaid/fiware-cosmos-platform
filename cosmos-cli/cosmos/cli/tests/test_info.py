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

from mock import MagicMock, patch

from cosmos.common.exceptions import ResponseError
from cosmos.common.tests.util import collect_outputs, mock_response
from cosmos.cli.info import info_command


class InfoCommandTest(unittest.TestCase):

    def setUp(self):
        self.config = MagicMock()
        self.config.api_url = 'http://host:port/some/api/v1'

    def test_show_general_info(self):
        response = mock_response(json={"general": "info"})
        with collect_outputs() as outputs, \
                patch('requests.get', MagicMock(return_value=response)):
            info_command([], self.config)
            self.assertIn(""""general": "info""""", outputs.stdout.getvalue())

    def test_cannot_show_general_info(self):
        response = mock_response(status_code=500)
        with patch('requests.get', MagicMock(return_value=response)):
            self.assertRaisesRegexp(
                ResponseError, 'Cannot get general information',
                info_command, 'cluster1', self.config)
