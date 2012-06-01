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
        self.nop_jar = JarFile(os.path.join(test_path, 'jar-nop.jar'))

    def tearDown(self):
        self.prop_jar.close()
        self.xml_jar.close()
        self.nop_jar.close()

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

    def test_is_parameterized(self):
        self.assertTrue(self.prop_jar.is_parameterized())
        self.assertTrue(self.xml_jar.is_parameterized())
        self.assertFalse(self.nop_jar.is_parameterized())

    def test_get_no_parametrization(self):
        self.assertRaises(ValueError, self.nop_jar.parameters)

    def test_get_parametrization_from_properties(self):
        params = self.prop_jar.parameters()
        self.assertEquals(len(params), 3)
        self.assertEquals(params[0].name, "foo")
        self.assertEquals(params[0].type, "string")
        self.assertEquals(params[0].default_value, None)
        self.assertEquals(params[1].name, "bar")
        self.assertEquals(params[1].default_value, "hola")
        self.assertEquals(params[2].name, "tmp")
        self.assertEquals(params[2].type, "filepath")

    def test_get_parametrization_from_xml(self):
        params = self.xml_jar.parameters()
        self.assertEquals(len(params), 3)
        self.assertEquals(params[0].name, "foo")
        self.assertEquals(params[1].default_value, "hola")
        self.assertEquals(params[2].type, "filepath")
