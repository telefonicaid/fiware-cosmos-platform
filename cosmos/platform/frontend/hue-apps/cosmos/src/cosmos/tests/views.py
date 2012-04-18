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


class DatasetsTestCase(test.TestCase):
    fixtures = ['users', 'sample_runs']

    def setUp(self):
        self.client.login(username='user101', password='user1')

    def test_datasets_listed(self):
        response = self.client.get('/cosmos/datasets/')
        self.assertEquals(response.status_code, 200)
        listed_names = map(lambda x: x.name, response.context['datasets'])
        self.assertTrue('dataset1' in listed_names,
                        msg='Own datasets should be listed')
        self.assertFalse('dataset3' in listed_names,
                         msg='Other user datasets should not be listed')


class CustomJarTestCase(test.TestCase):
    fixtures = ['users', 'sample_runs']

    def setUp(self):
        self.client.login(username='user101', password='user1')

    def test_datasets_listed(self):
        response = self.client.get('/cosmos/jars/')
        self.assertEquals(response.status_code, 200)
        listed_names = map(lambda x: x.name, response.context['jars'])
        self.assertTrue('wordcount' in listed_names,
                        msg='Own JARs should be listed')
        self.assertFalse('pi' in listed_names,
                         msg='Other user JARs should not be listed')
