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

from cosmos.cli.storage import add_storage_commands


class StorageCommandsTest(unittest.TestCase):

    def test_add_storage_commands(self):
        parser = MagicMock()
        add_storage_commands(parser)
        added_commands = [call[1][0] for call in parser.add_parser.mock_calls
                          if call[0] == '']
        self.assertIn('put', added_commands)
        self.assertIn('ls', added_commands)
        self.assertIn('get', added_commands)
        self.assertIn('rm', added_commands)
