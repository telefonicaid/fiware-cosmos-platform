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

import requests
from mock import MagicMock, patch
from testfixtures import TempDirectory

import cosmos.cli.config as config
from cosmos.cli.util import ExitWithError

class ConfigTest(unittest.TestCase):

    def test_load_no_config(self):
        with TempDirectory() as temp_dir:
            missing_config = MagicMock(
                return_value=temp_dir.getpath("nonexisting"))
            args = MagicMock()
            args.config_file = None
            with patch('cosmos.cli.config.default_config_path', missing_config):
                self.assertRaises(ExitWithError, config.load_config, args)

    def test_with_config(self):
        with patch('cosmos.cli.config.load_config',
                   MagicMock(return_value="config")):
            command = MagicMock(return_value="retval")
            decorated = config.with_config(command)
            self.assertEquals(decorated("arg"), "retval")
            command.assert_called_with("arg", "config")
