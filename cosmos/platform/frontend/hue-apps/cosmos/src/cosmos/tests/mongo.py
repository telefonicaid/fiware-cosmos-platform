# -*- coding: utf-8 -*-
"""Mongo connector tests."""

from django import test
from django.contrib.auth.models import User
from pymongo import Connection

from cosmos.models import JobRun
from cosmos.mongo import retrieve_results


class MongoTestCase(test.TestCase):
    fixtures = ['users']

    def setUp(self):
        self.job = JobRun(id=15, name='wordcount', user_id=101,
                          description="hadoop's hello world",
                          dataset_path='/user/jsmith/datasets/text.txt',
                          jar_path='/user/jsmith/jars/wordcount.jar')
        self.job.save()
        try:
            self.connection = Connection('mongodb://localhost')
        except:
            self.fail('Cannot connect to local mongodb test server')
        self.collection = self.connection['db_101']['job_15']
        self.collection.remove()
        self.collection.insert([{
            '_id': 'Hello',
            'word': 'Hello',
            'count': 1
        }, {
            '_id': 'world',
            'word': 'world',
            'count': 2
        }])

    def tearDown(self):
        self.collection.remove()
        self.connection.close()

    def test_retrieve_with_primary_key(self):
        results = retrieve_results(self.job.id, 'count')
        first_result = results.page(1).object_list[0]
        self.assertEquals(results.object_list.count(), 2)
        self.assertEquals('count', first_result.pk)
        self.assertEquals(1, first_result.get_primary_key())

    def test_retrieve_without_primary_key(self):
        results = retrieve_results(self.job.id)
        first_result = results.page(1).object_list[0]
        self.assertEquals(results.object_list.count(), 2)
        self.assertEquals('_id', first_result.pk)
        self.assertEquals('Hello', first_result.get_primary_key())
