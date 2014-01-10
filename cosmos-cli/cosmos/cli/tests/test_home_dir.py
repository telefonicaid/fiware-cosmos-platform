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
import os
import stat
import unittest

from testfixtures import TempDirectory

from cosmos.cli.home_dir import HomeDir
import cosmos.cli.home_dir as home_dir
from cosmos.common.tests.util import collect_outputs


class PathHomeDir(HomeDir):
    """Concrete class for testing the functionality of HomeDir"""

    def __init__(self, path):
        self.path = path

    def get_path(self):
        return self.path

    def get_default_config_filename(self):
        return ".cosmosrc"

    def get_last_cluster_filename(self):
        return ".cosmoslast"


class HomeDirTest(unittest.TestCase):

    def setUp(self):
        self.temp_dir = TempDirectory(create=True)
        self.home = PathHomeDir(self.temp_dir.path)

    def tearDown(self):
        self.temp_dir.cleanup()

    def test_read(self):
        self.temp_dir.write("filename", "contents")
        self.assertEquals(self.home.read("filename"), "contents")

    def test_write(self):
        self.temp_dir.write("existing_file", "existing_contents")
        self.home.write("new_file", "contents")
        self.home.write("existing_file", "new_contents")
        self.assertEquals(self.temp_dir.read("existing_file"),
                          "new_contents")
        self.assertEquals(self.temp_dir.read("new_file"), "contents")

    def test_config_file(self):
        with collect_outputs() as outputs:
            self.home.write_config_file("new config")
            self.temp_dir.check(".cosmosrc")
            self.assertEquals(self.home.read_config_file(), "new config")
            self.assertIn("Settings saved", outputs.stdout.getvalue())
            file_mode = os.stat(self.temp_dir.getpath(".cosmosrc")).st_mode
            self.assertEquals(file_mode, stat.S_IFREG | stat.S_IRUSR | stat.S_IWUSR)

    def test_override_config_file(self):
        with collect_outputs():
            other_config = self.temp_dir.write("path/other", "config")
            self.assertEquals(
                self.home.read_config_file(filename_override=other_config),
                "config")

    def test_warn_on_unprotected_config_file(self):
        with collect_outputs() as outputs:
            self.home.write_config_file("new config")
            config_path = self.temp_dir.getpath(".cosmosrc")
            os.lchmod(config_path, 0777)
            #import ipdb; ipdb.set_trace()
            self.home.read_config_file()
            self.assertIn("WARNING", outputs.stdout.getvalue())

    def test_last_cluster(self):
        self.home.write_last_cluster("0000000")
        self.temp_dir.check(".cosmoslast")
        self.assertEquals(self.home.read_last_cluster(), "0000000")


class SelectHomeDirTest(unittest.TestCase):

    def test_get_home_dir(self):
        self.assertIsNotNone(home_dir.get(), "Must select a home directory")
