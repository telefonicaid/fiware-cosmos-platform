# -*- coding: utf-8 -*-
"""
View tests.

"""
from django import test

from cosmos.models import JobRun


class JobRunsTestCase(test.TestCase):
    fixtures = ['users', 'sample_runs']
    
    def setUp(self):
        self.client.login(username='user101', password='user1')

    def test_jobruns_listed(self):
        response = self.client.get('/cosmos/')
        print response.context['job_runs']
        listed_names = map(lambda x: x.name, response.context['job_runs'])
        self.assertContains(response, 'running_wc', 
                            msg_prefix="Running jobs should be listed")
        self.assertContains(response, 'successful_wc',
                            msg_prefix="Successful jobs should be listed")
        self.assertContains(response, 'failed_wc',
                            msg_prefix="Failed jobs should be listed")
        self.assertNotContains(response, 'other_jobrun',
                               msg_prefix="Other user jobs should be unlisted")
