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
    job_id = 666
    fixtures = ['test.fixture.json']
    test_user = 'logc'
    test_pass = 'logc'

    def setUp(self):
        self.fake_results(self.job_id)

    def fake_results(self, job_id):
        new_results = [{"job_id" : job_id,
                        "word" : "Hello",
                        "count" :  1
                       },
                       {"job_id" : job_id,
                        "word" : "world",
                        "count" :  1
                       }] 
        connection = Connection('localhost', 27017)
        db = connection.test_database
        job_results = db.test_collection
        ignore = job_results.insert(new_results)

    def test_results_retrieved(self):
        c = Client()
        success = c.login(username=self.test_user, password=self.test_pass)
        response = c.get('/job/%s/results/' % self.job_id)
        self.assert_(response.status_code == 200)

