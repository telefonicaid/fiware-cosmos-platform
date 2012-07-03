# -*- coding: utf-8 -*-
"""Management commands tests."""
from django import test


class CreateUserTestCase(test.TestCase):
    fixtures = ['users']

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_create_user(self):
        pass
