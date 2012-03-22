"""
Module bdp_fe.jobconf.tests

This module holds the unittests for the bdp_fe.jobconf app
"""
from django.test import TestCase
from django.test.client import Client
from pymongo import Connection

class AlwaysTrue(TestCase):
    """
    AlwaysTrue is a dummy test case that always passes. This is just to prove
    that the testing framework is working.
    """
    def test_framework(self):
        '''This test case should always pass'''
        self.assert_(True)

class RetrieveFromMongo(TestCase):
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
        c = Client()
        success = c.login(username=self.test_user, password=self.test_pass)
        response = c.get('/job/%s/results/' % self.job_id)
        self.assertEquals(response.status_code, 200)
        self.assertEquals(len(response.context['job_results']), 2)
        for job_result in response.context['job_results']:
            if job_result['word'] == 'Hello':
                self.assertEquals(job_result['count'], 1)
            if job_result['word'] == 'world':
                self.assertEquals(job_result['count'], 1)
        c.logout()
