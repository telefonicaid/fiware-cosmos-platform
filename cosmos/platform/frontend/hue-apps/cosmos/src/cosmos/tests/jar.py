# -*- coding: utf-8 -*-
"""
JAR manipulation module tests.
"""
import os.path
import pickle
import unittest as test

from django.contrib.auth.models import User

from cosmos.expansion import ExpansionContext
from cosmos.models import JobRun
from cosmos.jar import InvalidJarFile, JarFile
from cosmos.jar_parameters import make_parameter


class CannotOpenJarTestCase(test.TestCase):

    def test_not_found(self):
        self.assertRaises(IOError, JarFile, 'does/not/exist.jar')

    def test_not_a_jar(self):
        self.assertRaises(InvalidJarFile, JarFile, __file__)


class UseJarTestCase(test.TestCase):

    def setUp(self):
        test_path = os.path.realpath(os.path.dirname(__file__))
        self.prop_jar = JarFile(os.path.join(test_path, 'jar-properties.jar'))
        self.xml_jar = JarFile(os.path.join(test_path, 'jar-xml.jar'))
        self.malformed_jar = JarFile(os.path.join(test_path,
                                                  'jar-malformed-xml.jar'))
        self.nop_jar = JarFile(os.path.join(test_path, 'jar-nop.jar'))

    def tearDown(self):
        self.prop_jar.close()
        self.xml_jar.close()
        self.malformed_jar.close()
        self.nop_jar.close()

    def test_read_manifest(self):
        manifest = self.prop_jar.manifest
        self.assertEquals(manifest['Cosmos-Parameters-Template'],
                                   '/conf/params.properties')
        self.assertTrue('not-a-key' not in manifest.keys())

    def test_read_long_manifest_lines(self):
        self.assertEquals(self.xml_jar.manifest['Cosmos-Parameters-Template'],
                '/conf/long-long-long-long-long-long-long-name-for-having-' +
                'more-than-one-line.xml')

    def test_is_parameterized(self):
        self.assertTrue(self.prop_jar.is_parameterized())
        self.assertTrue(self.xml_jar.is_parameterized())
        self.assertFalse(self.nop_jar.is_parameterized())

    def test_error_on_invalid_parametrization(self):
        self.assertRaises(InvalidJarFile, self.malformed_jar.parameters)

    def test_get_parametrization_from_properties(self):
        params = self.prop_jar.parameters()
        self.assertEquals(len(params), 5)
        self.assertEquals(params[0].name, "foo")
        self.assertEquals(params[0].default_value, None)
        self.assertEquals(params[1].name, "bar")
        self.assertEquals(params[1].default_value, "hola")
        self.assertEquals(params[2].name, "tmp")
        self.assertEquals(params[3].name, "mongo1")
        self.assertEquals(params[4].name, "mongo2")
        self.assertEquals(params[4].default_value, "col_a")

    def test_get_parametrization_from_xml(self):
        params = self.xml_jar.parameters()
        self.assertEquals(len(params), 3)
        self.assertEquals(params[0].name, "foo")
        self.assertEquals(params[1].default_value, "hola")


class JarParametersTestCase(test.TestCase):

    def setUp(self):
        self.foo = make_parameter('foo', 'string')
        self.bar = make_parameter('bar', 'string|value')
        self.tmp = make_parameter('tmp', 'filepath|/tmp')
        self.coll = make_parameter('coll', 'mongocoll')

    def test_pickable(self):
        for param in [self.foo, self.bar, self.tmp, self.coll]:
            serialized_param = pickle.dumps(param)
            deserialized_param = pickle.loads(serialized_param)
            self.assertEquals(param.name, deserialized_param.name)

    def test_invalid_default_value(self):
        self.assertRaises(ValueError, make_parameter, 'too_long',
                          'string|' + ('0123456789' * 30))
        self.assertRaises(ValueError, make_parameter, 'unsafe_path',
                          'filepath|../../etc/passwd')
        self.assertRaises(ValueError, make_parameter, 'invalid_chars',
                          'mongocoll|.&_$"')

    def test_argument_expansion(self):
        expansion = ExpansionContext()
        self.foo.set_value('hello_foo_${ job.id }', expansion)
        self.assertEquals(self.foo.as_job_argument(None, expansion),
                          ['-D', 'foo=hello_foo_0'])
        job = JobRun(id=15, user=User(id=7))
        self.assertEquals(self.coll.as_job_argument(job, expansion),
                          ['-D', 'coll=mongodb://localhost/db_7.job_0'])

