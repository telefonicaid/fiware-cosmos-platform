# -*- coding: utf-8 -*-
"""
Models tests.

"""
from django import test
from django.contrib.auth.models import User

from cosmos.models import Dataset, CustomJar


class DefaultPathTestCase(test.TestCase):
    """Check default paths on the HDFS tree"""

    def setUp(self):
        self.user = User(username="jsmith")

    def test_default_dataset_path(self):
        dataset = Dataset(user=self.user, name="data1")
        dataset.set_default_path()
        self.assertEquals('/user/jsmith/datasets/data1/', dataset.path)

    def test_default_jar_path(self):
        jar = CustomJar(user=self.user, name="wc_jar")
        jar.set_default_path()
        self.assertEquals('/user/jsmith/jars/wc_jar/', jar.path)
