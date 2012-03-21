"""
Module bdp_fe.jobconf.tests

This module holds the unittests for the bdp_fe.jobconf app
"""
from django.test import TestCase

class AlwaysTrue(TestCase):
    """
    AlwaysTrue is a dummy test case that always passes. This is just to prove
    that the testing framework is working.
    """
    def test_framework(self):
        '''This test case should always pass'''
        self.assert_(True)
