# -*- coding: utf-8 -*-
'''Expansion tests.'''
import unittest as test

from django.contrib.auth.models import User

from cosmos.expansion import ExpansionContext
from cosmos.models import JobRun


class ExpansionContextTestCase(test.TestCase):

    def setUp(self):
        job = JobRun(id=17, name='CustomJob')
        user = User(id=101, username='user1')
        self.context = ExpansionContext(job=job, user=user)
        self.jobless_context = ExpansionContext(user=user)
        self.userless_context = ExpansionContext(job=job)

    def test_expansion(self):
        self.assertEquals(self.context.expand('coll${job.id}'), 'coll17')
        self.assertEquals(self.context.expand('coll${ job.id }'), 'coll17')
        self.assertEquals(self.context.expand('${job.id}coll${job.id}'),
                          '17coll17')
        self.assertEquals(self.context.expand('coll${ job.id'), 'coll${ job.id')

    def test_unknown_variable_expansion(self):
        self.assertRaises(ValueError, self.context.expand,
                          '${unknown} variable')

    def test_invalid_variable_expansion(self):
        self.assertRaises(ValueError, self.context.expand, '${${}} variable')

    def test_job_expansions(self):
        self.assertEquals(self.context.expand('coll${job.id}'), 'coll17')
        self.assertEquals(self.context.expand('Starting ${job.name}'),
                          'Starting CustomJob')
        self.assertEquals(self.jobless_context.expand('coll${job.id}'), 'coll0')
        self.assertEquals(self.jobless_context.expand('Starting ${job.name}'),
                          'Starting jobname')

    def test_user_expansions(self):
        self.assertEquals(self.context.expand("${user.name}'s job"),
                          "user1's job")
        self.assertEquals(self.context.expand('${user.home}/output'),
                          '/user/user1/output')
        self.assertEquals(self.userless_context.expand("${user.name}'s job"),
                          "username's job")
        self.assertEquals(self.userless_context.expand('${user.home}/output'),
                          '/user/username/output')

    def test_decorate_function(self):
        deco_int = self.context.decorate(int)
        self.assertEquals(deco_int('${ job.id }'), 17)
