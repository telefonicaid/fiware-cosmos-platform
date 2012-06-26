# -*- coding: utf-8 -*-
"""
View tests.

"""
from django import test
from hadoop.fs import LocalSubFileSystem
from pymongo import Connection

from cosmos.models import JobRun

class JobRunsTestCase(test.TestCase):
    fixtures = ['users', 'sample_runs']

    def setUp(self):
        self.client.login(username='user101', password='user1')

    def test_jobruns_listed(self):
        response = self.client.get('/cosmos/')
        listed_names = [x.name for x in response.context['job_runs']]
        self.assertEquals(response.status_code, 200)
        self.assertTrue('running_wc' in listed_names,
                        msg="Running jobs should be listed")
        self.assertTrue('successful_wc' in listed_names,
                        msg="Successful jobs should be listed")
        self.assertTrue('failed_wc' in listed_names,
                        msg="Failed jobs should be listed")
        self.assertFalse('other_jobrun' in listed_names,
                        msg="Other user jobs should be unlisted")


class ViewSuccessfulResultsTestCase(test.TestCase):
    fixtures = ['users', 'sample_runs']

    def setUp(self):
        self.client.login(username='user101', password='user1')
        try:
            self.connection = Connection('mongodb://localhost')
        except:
            self.fail('Cannot connect to local mongodb test server')
        self.collection = self.connection['db_101']['job_2']
        self.collection.remove()
        self.collection.insert([{
            'word': 'Hello',
            'count': 1
        }, {
            'word': 'world',
            'count': 2
        }])

    def tearDown(self):
        self.collection.remove()
        self.connection.close()

    def test_display_results(self):
        response = self.client.get('/cosmos/job/2/results/?primary_key=word')
        self.assertEquals(response.status_code, 200)
        results = response.context['page'].object_list
        self.assertEquals(len(results), 2)
        self.assertEquals(results[0].pk, 'word')
