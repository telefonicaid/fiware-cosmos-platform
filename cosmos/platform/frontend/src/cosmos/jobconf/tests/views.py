"""
Views tests

"""
from django.conf import settings
import django.contrib.messages as msg
from django.contrib.auth.models import User
from django.utils import unittest
import django.test as djangotest
from pymongo import Connection

from cosmos.jobconf.models import CustomJobModel, Job


class LoginTestCase(djangotest.TestCase):
    fixtures = ['sample_jobs']

    def test_login_redirect(self):
        response = self.client.get('/')
        self.assertRedirects(response, '/accounts/login/?next=/')

    def test_successful_login(self):
        response = self.client.post('/accounts/login/?next=/', {
            'username': 'user1', 'password': 'user1'})
        self.assertRedirects(response, '/')


class ViewTestCase(djangotest.TestCase):
    """Additional assertions for testing views"""

    def assertMessage(self, response, level, submsg=None):
        """Assert that an error or at least a given level is present in
           a response.

           Optionally, error message should include submsg content."""

        messages = response.context['messages']
        explanation = "Should have a message of level>=%d" % level
        if submsg is not None:
            explanation = explanation + " and text '%s'" % submsg
        matches_assertion = lambda m: (
            (m.level >= level) and ((submsg is None) or
                                    (m.message.lower().find(submsg.lower())
                                     >= 0)))
        self.assertTrue(any(filter(matches_assertion, messages)),
                        msg=explanation)


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
        settings.CLUSTER_CONF['connection-factory'] = (
            'cosmos.jobconf.cluster.fakeserver.ClusterHandler')
        self.client.login(username='user1', password='user1')

    def expect_job_failure(self, job_id, expected_level, expected_message):
        response = self.client.get('/?run_job=%d' % job_id)
        self.assertEquals(response.status_code, 200)
        self.assertMessage(response, expected_level, expected_message)

    def test_job_start_failure(self):
        self.expect_job_failure(1234, msg.WARNING, "not found")
        self.expect_job_failure(   8, msg.WARNING, "not found")
        self.expect_job_failure(   2, msg.WARNING, "unconfigured")
        self.expect_job_failure(   3, msg.WARNING, "no data for running")
        self.expect_job_failure(   5, msg.WARNING, "running")
        self.expect_job_failure(   6, msg.WARNING, "failed")
        self.expect_job_failure(   7, msg.WARNING, "successful")

    def test_job_start_success(self):
        response = self.client.get('/?run_job=4')
        self.assertEquals(response.status_code, 200)
        self.assertMessage(response, msg.INFO, 'was started')


class JobResultsTestCase(ViewTestCase):
    fixtures = ['sample_jobs']

    def setUp(self):
        self.client.login(username='user1', password='user1')

    def expect_status(self, job_id, expected_status):
        response = self.client.get('/job/%d/results/' % job_id)
        self.assertEquals(response.status_code, expected_status)

    def test_for_unexisting_job(self):
        self.expect_status(1234, 404)

    def test_403_for_not_owned_job(self):
        self.expect_status(8, 403)

    def test_404_for_unfinished_job(self):
        self.expect_status(2, 404) # unconfigured
        self.expect_status(3, 404) # no data
        self.expect_status(4, 404) # ready to run
        self.expect_status(5, 404) # running

    def test_failure_report(self):
        response = self.client.get('/job/6/results/')
        self.assertEquals(response.status_code, 200)
        self.assertTrue(response.context['title'].startswith('Error report'))

    def test_success_report(self):
        response = self.client.get('/job/7/results/')
        self.assertEquals(response.status_code, 200)
        self.assertTrue(response.context['title'].startswith('Results of job'))


class RetrieveFromMongo(djangotest.TestCase):
    """
    RetrieveFromMongo is a test where some records are written to a test
    collection, and then read back into the view_results view.

    Note: a local mongodb server MUST be running for this test case to pass.
    """
    fixtures = ['test.fixture.json']

    def setUp(self):
        self.job_name = 'not_very_useful'
        self.mongo_host = 'localhost'
        self.mongo_port = 27017
        self.test_user = 'test'
        self.test_pass = 'test'
        self.job_id = self.__create_job()

        self.connection = Connection(self.mongo_host, self.mongo_port)
        self.db = self.connection[self.test_user]
        self.job_results = self.db['job_%s' % self.job_id]
        self.__fake_results(self.job_id)

    def tearDown(self):
        self.job_results.remove()

    def __create_job(self):
        user = User.objects.get(username=self.test_user)
        myjob = Job(name=self.job_name,
                    user=user,
                    status=Job.SUCCESSFUL)
        myjob.save()
        mymodel = CustomJobModel(job=myjob)
        mymodel.jar_name = 'non-existant'
        myjob.jobmodel = mymodel
        mymodel.save()
        return myjob.id

    def __fake_results(self, job_id):
        new_results = [{
                        "word" : "Hello",
                        "count" :  1
                       },
                       {
                        "word" : "world",
                        "count" :  1
                       }]
        self.job_results.insert(new_results)

    def test_results_retrieved(self):
        success = self.client.login(username=self.test_user,
                                    password=self.test_pass)
        response = self.client.get('/job/%s/results/?primary_key=word' % self.job_id)
        self.assertEquals(response.status_code, 200)
        job_results = response.context['job_results'].object_list
        self.assertEquals(len(job_results), 2)
        for job_result in job_results:
            if job_result.get_primary_key() == 'Hello':
                self.assertEquals(job_result.get_fields()['count'], 1)
            if job_result.get_primary_key() == 'world':
                self.assertEquals(job_result.get_fields()['count'], 1)
        self.client.logout()
