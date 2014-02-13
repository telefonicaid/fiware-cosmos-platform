# -*- coding: utf-8 -*-
#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED 'AS IS' WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
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
