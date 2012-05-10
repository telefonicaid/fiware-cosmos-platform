# -*- coding: utf-8 -*-
"""
Models tests.

"""
from django import test
from django.contrib.auth.models import User

from cosmos.models import JobRun


class JobRunTestCase(test.TestCase):

    def setUp(self):
        self.job = JobRun(id=15, name='wordcount', 
                          description="hadoop's hello world",
                          user=User(id=1, username='jsmith'),
                          dataset_path='/user/jsmith/datasets/text.txt',
                          jar_path='/user/jsmith/jars/wordcount.jar')

    def test_hadoop_args(self):
        self.assertEquals(self.job.hadoop_args('job.jar'),
                          ['jar', 'job.jar', '/user/jsmith/datasets/text.txt',
                           '/user/jsmith/tmp/job_15/',
                           'mongodb://localhost/db_1.job_15'])
