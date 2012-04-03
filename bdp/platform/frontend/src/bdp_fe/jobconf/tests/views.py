"""
Views tests

"""
from django.utils import unittest

import django.test as djangotest
from pymongo import Connection


class LoginTestCase(djangotest.TestCase):

    def test_login_redirect(self):
        response = self.client.get('/')
        self.assertRedirects(response, '/accounts/login/?next=/')


class RetrieveFromMongo(djangotest.TestCase):
    """
    RetrieveFromMongo is a test where some records are written to a test
    collection, and then read back into the view_results view.

    Note: mongodb server MUST be running for this test case to pass.
    """
    fixtures = ['test.fixture.json']

    def setUp(self):
        self.job_id = 666
        self.mongo_host = 'localhost'
        self.mongo_port = 27017
        self.test_user = 'test'
        self.test_pass = 'test'
        self.connection = Connection(self.mongo_host, self.mongo_port)
        self.db = self.connection.test_database
        self.job_results = self.db.test_collection
        self.fake_results(self.job_id)

    def tearDown(self):
        self.job_results.remove()

    def fake_results(self, job_id):
        new_results = [{"job_id" : job_id,
                        "word" : "Hello",
                        "count" :  1
                       },
                       {"job_id" : job_id,
                        "word" : "world",
                        "count" :  1
                       }]
        self.job_results.insert(new_results)

    def test_results_retrieved(self):
        success = self.client.login(username=self.test_user,
                                    password=self.test_pass)
        response = self.client.get('/job/%s/results/' % self.job_id)
        self.assertEquals(response.status_code, 200)
        job_results = response.context['job_results'].object_list
        self.assertEquals(len(job_results), 2)
        for job_result in job_results:
            if job_result['word'] == 'Hello':
                self.assertEquals(job_result['count'], 1)
            if job_result['word'] == 'world':
                self.assertEquals(job_result['count'], 1)
        self.client.logout()
