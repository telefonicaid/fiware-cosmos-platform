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
