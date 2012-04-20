"""
Views tests

"""
from django import test
from django.contrib.auth.models import User
from django.utils import unittest

from bdp_fe.jobconf.models import CustomJobModel, Job

class CustomJobTestCase(test.TestCase):

    def test_mongo_url(self):
        user1 = User(pk=1, username="user1")
        job = Job(pk=58, name="a_job", user=user1)
        model = CustomJobModel(job=job)

        self.assertEquals(model.mongo_base(), "mongodb://localhost")
        self.assertEquals(model.mongo_db(), "db_1")
        self.assertEquals(model.mongo_collection(), "job_58")
        self.assertEquals(model.mongo_url(), "mongodb://localhost/db_1.job_58")
