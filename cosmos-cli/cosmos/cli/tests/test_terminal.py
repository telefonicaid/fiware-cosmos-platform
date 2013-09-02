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
