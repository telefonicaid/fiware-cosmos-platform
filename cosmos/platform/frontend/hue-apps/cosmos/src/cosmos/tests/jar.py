# -*- coding: utf-8 -*-
"""
JAR manipulation module tests.
"""
import os.path
import unittest as test

from cosmos.expansion import ExpansionContext
from cosmos.jar import InvalidJarFile, JarFile, ParameterTemplate
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
        self.assertRaises(InvalidJarFile, self.malformed_jar.parameter_template)

    def test_get_parametrization_from_properties(self):
        params = self.prop_jar.parameter_template().parameters
        self.assertEquals(len(params), 5)
        self.assertEquals(params[0].name, "foo")
        self.assertEquals(params[0].default_value, None)
        self.assertEquals(params[1].name, "bar")
        self.assertEquals(params[1].default_value, "hola")
        self.assertEquals(params[2].name, "tmp")
        self.assertEquals(params[2].default_value,
                          "${ user.home }/tmp/run${ job.id }")
        self.assertEquals(params[3].name, "mongo1")
        self.assertEquals(params[4].name, "mongo2")
        self.assertEquals(params[4].default_value, "col_a")

    def test_get_parametrization_from_xml(self):
        params = self.xml_jar.parameter_template().parameters
        self.assertEquals(len(params), 3)
        self.assertEquals(params[0].name, "foo")
        self.assertEquals(params[1].default_value, "hola")


class ParameterTemplateTestCase(test.TestCase):

    def setUp(self):
        self.instance = ParameterTemplate()
        self.instance.add(make_parameter('string1', 'string'))
        self.instance.add(make_parameter('string2', 'string|default'))
        self.expansion = ExpansionContext()

    def test_error_on_duplicates(self):
        self.assertRaises(InvalidJarFile, self.instance.add,
                          make_parameter('string1', 'mongocoll'))

    def test_update_from_form(self):
        self.instance.update_from_form({
            'param0': 'hello',
            'param1': None
        })
        self.assertEquals(['-D', 'string1=hello', '-D', 'string2=default'],
                          self.instance.as_hadoop_args(None, self.expansion))


class ParameterizeFormTestCase(test.TestCase):
    """
    Test dynamic form generation.
    """

    def setUp(self):
        template = ParameterTemplate()
        template.add(make_parameter('foo', 'string'))
        template.add(make_parameter('bar', 'filepath|/tmp/'))
        self.expansion = ExpansionContext()
        self.form = template.as_form(self.expansion)

    def test_generate_fields(self):
        self.assertEquals([(name, type(field).__name__) for (name, field)
                           in self.form.fields.items()],
                          [('param0', 'CharField'),
                           ('param1', 'CharField')])
        self.assertEquals(type(self.form.fields['param1'].widget).__name__,
                          'HDFSFileChooser')

    def test_use_default_values(self):
        self.assertEquals(self.form.fields['param1'].initial, '/tmp/')
