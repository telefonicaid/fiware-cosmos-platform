# -*- coding: utf-8 -*-
"""
View tests.

"""
from os import path
import tempfile
import shutil

from django import test
from hadoop.fs import LocalSubFileSystem
from desktop.lib import fsmanager

from cosmos.models import JobRun


class JobRunsTestCase(test.TestCase):
    fixtures = ['users', 'sample_runs']

    def setUp(self):
        self.client.login(username='user101', password='user1')

    def test_jobruns_listed(self):
        response = self.client.get('/cosmos/')
        listed_names = map(lambda x: x.name, response.context['job_runs'])
        self.assertEquals(response.status_code, 200)
        self.assertTrue('running_wc' in listed_names,
                        msg="Running jobs should be listed")
        self.assertTrue('successful_wc' in listed_names,
                        msg="Successful jobs should be listed")
        self.assertTrue('failed_wc' in listed_names,
                        msg="Failed jobs should be listed")
        self.assertFalse('other_jobrun' in listed_names,
                        msg="Other user jobs should be unlisted")
