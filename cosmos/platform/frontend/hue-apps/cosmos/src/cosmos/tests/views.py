# -*- coding: utf-8 -*-
"""
View tests.

"""
from django import test
from django.contrib.auth.models import User
from jobsub.models import Submission
from pymongo import Connection

from cosmos.expansion import ExpansionContext
from cosmos.jar_parameters import make_parameter
from cosmos.models import JobRun
from cosmos.views import hadoop_args


class JobRunsTestCase(test.TestCase):
    fixtures = ['users', 'sample_runs']

    def setUp(self):
        self.client.login(username='user101', password='user1')

    def test_jobruns_listed(self):
        response = self.client.get('/cosmos/')
        listed_names = [x.name for x in response.context['job_runs']]
        self.assertEquals(response.status_code, 200)
        self.assertTrue('running_wc' in listed_names,
                        msg="Running jobs should be listed")
        self.assertTrue('successful_wc' in listed_names,
                        msg="Successful jobs should be listed")
        self.assertTrue('failed_wc' in listed_names,
                        msg="Failed jobs should be listed")
        self.assertFalse('other_jobrun' in listed_names,
                        msg="Other user jobs should be unlisted")


class HadoopArgsTestCase(test.TestCase):

    def setUp(self):
        self.job = JobRun(id=15, name='wordcount',
                          description="hadoop's hello world",
                          user=User(id=1, username='jsmith'),
                          dataset_path='/user/jsmith/datasets/text.txt',
                          jar_path='/user/jsmith/jars/wordcount.jar',
                          submission=Submission(id=23))
        self.expansion = ExpansionContext(job=self.job,
                                          user=User(id=101, username="user1"))

    def test_hadoop_args(self):
        self.assertEquals(hadoop_args(self.job, 'job.jar', self.expansion),
                          ['jar', 'job.jar', '/user/jsmith/datasets/text.txt',
                           '/user/jsmith/tmp/job_15/',
                           'mongodb://localhost/db_1.job_15'])

    def test_hadoop_args_with_parameters(self):
        custom = make_parameter('output', 'filepath')
        custom.set_value('${user.home}/output', self.expansion)
        self.job.parameters = [make_parameter('foo', 'string|bar'),
                               make_parameter('mongo1', 'mongocoll|col_a'),
                               custom]
        self.assertEquals(hadoop_args(self.job, 'job.jar', self.expansion),
                          ['jar', 'job.jar',
                           '-D', 'foo=bar',
                           '-D', 'mongo1=mongodb://localhost/db_1.col_a',
                           '-D', 'output=/user/user1/output'])


def local_mongo():
    try:
        return Connection('mongodb://localhost')
    except:
        raise Exception('Cannot connect to local mongodb test server')


def ensure_empty_coll(database, coll_name):
    """
    Returns an empty collection by creating or truncating it as necessary.
    """
    if coll_name in database.collection_names():
        collection = database[coll_name]
        collection.remove()
        return collection
    else:
        return database.create_collection(coll_name)


class ViewSuccessfulResultsTestCase(test.TestCase):
    fixtures = ['users', 'sample_runs']

    def setUp(self):
        self.client.login(username='user101', password='user1')
        self.connection = local_mongo()
        self.collection = self.connection['db_101']['job_2']
        self.collection.remove()
        self.collection.insert([{
            'word': 'Hello',
            'count': 1
        }, {
            'word': 'world',
            'count': 2
        }])

    def tearDown(self):
        self.collection.remove()
        self.connection.close()

    def test_display_results(self):
        response = self.client.get('/cosmos/results/job_2/?primary_key=word')
        self.assertEquals(response.status_code, 200)
        results = response.context['page'].object_list
        self.assertEquals(len(results), 2)
        self.assertEquals(results[0].pk, 'word')


class ListResultsTestCase(test.TestCase):
    fixtures = ['users']

    def setUp(self):
        self.client.login(username='user101', password='user1')
        self.connection = local_mongo()

        self.my_db = self.connection['db_101']
        ensure_empty_coll(self.my_db, 'foo123')
        ensure_empty_coll(self.my_db, 'bar456')

        self.other_db = self.connection['db_102']
        ensure_empty_coll(self.other_db, 'baz789')

    def test_list_collections(self):
        response = self.client.get('/cosmos/results/')
        listed_names = [x.name for x in response.context['collections']]
        self.assertEquals(response.status_code, 200)
        self.assertTrue('foo123' in listed_names and 'bar456' in listed_names,
                        msg="Collections owned by user should be listed")
        self.assertTrue('baz789' not in listed_names,
                        msg="Collections owned by other users should not be listed")

