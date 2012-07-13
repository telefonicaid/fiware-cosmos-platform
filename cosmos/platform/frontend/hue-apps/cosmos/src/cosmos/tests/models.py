# -*- coding: utf-8 -*-
"""
Models tests.

"""
from django import test
from django.contrib.auth.models import User
from jobsub.models import Submission
from jobsubd.ttypes import State

from cosmos.models import JobRun


class JobRunTestCase(test.TestCase):

    def setUp(self):
        self.job = JobRun(id=15, name='wordcount',
                          description="hadoop's hello world",
                          user=User(id=1, username='jsmith'),
                          dataset_path='/user/jsmith/datasets/text.txt',
                          jar_path='/user/jsmith/jars/wordcount.jar',
                          submission=Submission(id=23))

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
                                    'href': '/cosmos/results/job_15/'},
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
