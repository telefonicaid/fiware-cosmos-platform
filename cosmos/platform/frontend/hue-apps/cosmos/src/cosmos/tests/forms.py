# -*- coding: utf-8 -*-
"""
Forms tests.

"""
import unittest as test

from cosmos.forms import ParameterizeJobForm


class ParameterizeFormTestCase(test.TestCase):
    def setUp(self):
        self.parameters = [{
            'name': 'foo',
            'type': 'string'
        }, {
            'name': 'bar',
            'type': 'filepath',
            'default_value': '/tmp/'
        }]

    def test_generate_fields(self):
        form = ParameterizeJobForm(self.parameters)

        self.assertEquals([(name, type(field).__name__) for (name, field)
                           in form.fields.items()],
                          [('foo', 'CharField'),
                           ('bar', 'CharField')])
        self.assertEquals(type(form.fields['bar'].widget).__name__, 
                          'HDFSFileChooser')

    def test_use_default_values(self):
        form = ParameterizeJobForm(self.parameters)
        self.assertEquals(form.fields['bar'].initial, '/tmp/')
