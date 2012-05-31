# -*- coding: utf-8 -*-
"""
JAR manipulation module tests.

"""

import os.path
import unittest as test

from cosmos.jar import InvalidJarFile, JarFile


class CannotOpenJarTestCase(test.TestCase):

    def test_not_fount(self):
        self.assertRaises(IOError, JarFile, 'does/not/exist.jar')

    def test_not_a_jar(self):
        self.assertRaises(InvalidJarFile, JarFile, __file__)


class UseJarTestCase(test.TestCase):

    def setUp(self):
        test_path = os.path.realpath(os.path.dirname(__file__))
        self.prop_jar = JarFile(os.path.join(test_path, 'jar-properties.jar'))
        self.xml_jar = JarFile(os.path.join(test_path, 'jar-xml.jar'))

    def tearDown(self):
        self.prop_jar.close()
        self.xml_jar.close()

    def test_read_manifest(self):
        manifest = self.prop_jar.manifest()
        self.assertEquals(manifest['Cosmos-Parameters-Template'],
                                   '/conf/params.properties')
        self.assertTrue('not-a-key' not in manifest.keys())

    def test_read_long_manifest_lines(self):
        manifest = self.xml_jar.manifest()
        self.assertEquals(manifest['Cosmos-Parameters-Template'],
                                   '/conf/long-long-long-long-long-long-' + 
                                   'long-name-for-having-more-than-one-' +
                                   'line.xml')
