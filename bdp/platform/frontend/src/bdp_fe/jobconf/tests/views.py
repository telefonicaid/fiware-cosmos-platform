"""
Views tests

"""
from django.contrib.auth.models import User, UserManager
import django.test as djangotest
from django.utils import unittest
from pymongo import Connection

from bdp_fe.jobconf.models import CustomJobModel, Job

class LoginTestCase(djangotest.TestCase):

    def test_login_redirect(self):
        response = self.client.get('/')
        self.assertRedirects(response, '/accounts/login/?next=/')

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
