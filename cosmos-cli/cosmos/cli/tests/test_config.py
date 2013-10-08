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

import cosmos.cli.config as config
from cosmos.cli.util import ExitWithError
from cosmos.cli.tests.mock_home_dir import mock_home
from cosmos.common.tests.util import (collect_outputs,
                                      mocked_standard_descriptors)


class ConfigTest(unittest.TestCase):

    def setUp(self):
        self.args = MagicMock()
        self.args.config_file = None
        self.interactive_configuration = """http://host/cosmos/v1
        keykeykeyk
        secretsecretsecretse
        ssh
        """

    def test_load_no_config(self):
        with mock_home():
            self.assertRaises(ExitWithError, config.load_config, self.args)

    def test_load_malformed_config(self):
        with mock_home(config="malformed"), collect_outputs() as outputs:
            self.assertRaises(ExitWithError, config.load_config, self.args)
            self.assertIn("Error reading configuration",
                          outputs.stdout.getvalue())

    def test_with_config(self):
        sample_conf = {
            "api_key": "AAAA",
            "api_secret": "BBBB",
            "api_url": "http://localhost/cosmos/v1",
            "ssh": "ssh"
        }
        with mock_home(config=sample_conf):
            def command(args, name, config):
                return "%s-%s" % (name, config.api_key)
            decorated = config.with_config(command)
            self.assertEquals(decorated(self.args, "command1"), "command1-AAAA")

    def test_config_command(self):
        with mocked_standard_descriptors(self.interactive_configuration), \
                mock_home() as home:
            self.assertEquals(config.command(self.args), 0)
            self.assertIn("api_key: keykeykeyk", home.read_config_file())
