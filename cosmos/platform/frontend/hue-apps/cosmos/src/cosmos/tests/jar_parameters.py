# -*- coding: utf-8 -*-
"""
Forms tests.

"""
import pickle
import unittest as test

from django.contrib.auth.models import User

from cosmos.expansion import ExpansionContext
from cosmos.jar_parameters import make_parameter
from cosmos.models import JobRun


class JarParametersTestCase(test.TestCase):

    def setUp(self):
        self.foo = make_parameter('foo', 'string')
        self.bar = make_parameter('bar', 'string|value')
        self.tmp = make_parameter('tmp', 'filepath|/tmp')
        self.coll = make_parameter('coll', 'mongocoll')

    def test_set_value_is_not_validated(self):
        self.foo.set_value('too long' * 125)
        self.tmp.set_value('/..')
        self.coll.set_value('678')

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
        self.foo.set_value('hello_foo_${ job.id }')
        self.assertEquals(self.foo.as_job_argument(None, expansion),
                          ['-D', 'foo=hello_foo_0'])
        job = JobRun(id=15, user=User(id=7))
        self.assertEquals(self.coll.as_job_argument(job, expansion),
                          ['-D', 'coll=mongodb://localhost/db_7.job_0'])
