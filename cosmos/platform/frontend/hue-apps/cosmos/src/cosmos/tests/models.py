# -*- coding: utf-8 -*-
"""
Models tests.

"""
from django import test
from django.contrib.auth.models import User
from jobsubd.ttypes import State
from jobsub.models import Submission

from cosmos.jar_parameters import make_parameter
from cosmos.models import JobRun


class JobRunTestCase(test.TestCase):

    def setUp(self):
        self.job = JobRun(id=15, name='wordcount',
                          description="hadoop's hello world",
                          user=User(id=1, username='jsmith'),
                          dataset_path='/user/jsmith/datasets/text.txt',
                          jar_path='/user/jsmith/jars/wordcount.jar',
                          submission=Submission(id=23))

    def test_hadoop_args(self):
        self.assertEquals(self.job.hadoop_args('job.jar'),
                          ['jar', 'job.jar', '/user/jsmith/datasets/text.txt',
                           '/user/jsmith/tmp/job_15/',
                           'mongodb://localhost/db_1.job_15'])

    def test_hadoop_args_with_parameters(self):
        self.job.parameters = [make_parameter('foo', 'string|bar'),
                               make_parameter('mongo1', 'mongocoll|col_a')]
        self.assertEquals(self.job.hadoop_args('job.jar'),
                          ['jar', 'job.jar',
                           '-D', 'foo=bar',
                           '-D', 'mongo1=mongodb://localhost/db_1.col_a'])

    def assert_link_in_states(self, link, states):
        for state in states:
            self.job.submission.last_seen_state = state
            links = self.job.action_links()
            self.assertTrue(link in links, msg=('Should have "%s" link in "%s"' +
                            ' state (actual links: %s)') %
                            (link['name'], self.job.state(), links))


    def test_action_links(self):
        self.assert_link_in_states({'name': 'Results',
                                    'class': 'results',
                                    'target': None,
                                    'href': '/cosmos/job/15/results/'},
                                   [State.SUCCESS])

        self.assert_link_in_states({'name': 'Detailed status',
                                    'class': 'status',
                                    'target': 'JobSub',
                                    'href': '/jobsub/watch/23'},
                                   [State.SUBMITTED, State.RUNNING, State.SUCCESS,
                                    State.ERROR, State.FAILURE])

    def test_no_links_when_unsubmitted(self):
        self.job.submission = None
        self.assertEquals(self.job.action_links(), [])
