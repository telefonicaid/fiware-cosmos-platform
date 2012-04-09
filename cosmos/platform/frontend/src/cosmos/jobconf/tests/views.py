"""
Views tests

"""
from django.contrib import messages
from django.utils import unittest

import django.test as djangotest
from pymongo import Connection


class LoginTestCase(djangotest.TestCase):

    def test_login_redirect(self):
        response = self.client.get('/')
        self.assertRedirects(response, '/accounts/login/?next=/')

    def test_successful_login(self):
        response = self.client.post('/accounts/login/?next=/', {
            'username': 'admin',
            'password': 'du7rkwhu'})
        self.assertRedirects(response, '/')


class ViewTestCase(djangotest.TestCase):
    """Additional assertions for testing views"""

    def assertMessage(self, response, level, submsg=None):
        """Assert that an error or at least a given level is present in
           a response.

           Optionally, error message should include submsg content."""

        reported = response.context['messages']
        explanation = "Should have a message of level>=%d" % level
        if submsg is not None:
            explanation = explanation + " and text '%s'" % submsg
        criteria = lambda m: ((m.level >= level) and
                              ((submsg is None) or
                               (m.message.lower().find(submsg.lower()) >= 0)))
        self.assertTrue(any(filter(criteria, reported)), msg=explanation)


class JobListingTestCase(djangotest.TestCase):
    fixtures = ['sample_jobs']

    def setUp(self):
        self.client.login(username='user1', password='user1')

    def test_jobs_listed(self):
        response = self.client.get('/')
        self.assertEquals(response.status_code, 200)
        listed_jobs = map(lambda x: x.name, response.context['jobs'])
        self.assertNotIn('other_user_job', listed_jobs,
                         msg="Other user jobs should be unlisted");
        self.assertNotIn('unconfigured', listed_jobs,
                         msg="Unconfigured jobs should be unlisted");
        self.assertIn('no_data', listed_jobs,
                      msg="Jobs without data should be listed");
        self.assertIn('running', listed_jobs,
                      msg="Running jobs should be listed");
        self.assertIn('failed', listed_jobs,
                      msg="Failed jobs should be listed");
        self.assertIn('successful', listed_jobs,
                      msg="Successful jobs should be listed");


class JobStartTestCase(ViewTestCase):
    fixtures = ['sample_jobs']

    def setUp(self):
        self.client.login(username='user1', password='user1')

    def expect_job_failure(self, job_id, expected_level, expected_message):
        response = self.client.get('/?run_job=%d' % job_id)
        self.assertEquals(response.status_code, 200)
        self.assertMessage(response, expected_level, expected_message)

    def test_job_start_failure(self):
        self.expect_job_failure(1234, messages.WARNING, "not found")
        self.expect_job_failure(   8, messages.WARNING, "not found")
        self.expect_job_failure(   2, messages.WARNING, "unconfigured")
        self.expect_job_failure(   3, messages.WARNING, "no data for running")
        self.expect_job_failure(   5, messages.WARNING, "running")
        self.expect_job_failure(   6, messages.WARNING, "failed")
        self.expect_job_failure(   7, messages.WARNING, "successful")

    def test_job_start_success(self):
        response = self.client.get('/?run_job=4')
        self.assertEquals(response.status_code, 200)
        self.assertMessage(response, messages.INFO, 'was started')


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
