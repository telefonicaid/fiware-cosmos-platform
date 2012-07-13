# -*- coding: utf-8 -*-
"""Mongo connector tests."""
from datetime import datetime

from django import test
from pymongo import Connection

from cosmos.models import JobRun
from cosmos.mongo import retrieve_results, list_collections


class MongoIntegrationTestCase(test.TestCase):
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
        results = retrieve_results(self.job.user_id, 'job_15', 'count')
        first_result = results.page(1).object_list[0]
        self.assertEquals(results.object_list.count(), 2)
        self.assertEquals('count', first_result.pk)
        self.assertEquals(1, first_result.get_primary_key())

    def test_retrieve_without_primary_key(self):
        results = retrieve_results(self.job.user_id, 'job_15')
        first_result = results.page(1).object_list[0]
        self.assertEquals(results.object_list.count(), 2)
        self.assertEquals('_id', first_result.pk)
        self.assertEquals('Hello', first_result.get_primary_key())

    def test_list_existing(self):
        collections = list_collections(101)
        names = [c.name for c in collections]
        self.assertTrue('job_15' in names,
                        msg="Should list existing collections")


class MongoModificationTimeTestCase(test.TestCase):

    def setUp(self):
        try:
            self.connection = Connection('mongodb://localhost')
        except:
            self.fail('Cannot connect to local mongodb test server')
        database = self.connection['db_123456']
        col1 = database['col1']
        col1.remove({})
        col1.insert({'new': 'document'})

    def test_modification_time(self):
        collections = list_collections(123456)

        col1 = None
        for col in collections:
            if col.name == 'col1':
                col1 = col
        self.assertTrue(col1 is not None, msg='Should list col1')

        now = datetime.now()
        self.assertEquals(col1.timestamp.year, now.year)
        self.assertEquals(col1.timestamp.month, now.month)
        self.assertEquals(col1.timestamp.day, now.day)

    def tearDown(self):
        self.connection.close()
