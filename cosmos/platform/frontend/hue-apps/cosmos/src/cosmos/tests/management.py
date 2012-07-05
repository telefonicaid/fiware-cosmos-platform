# -*- coding: utf-8 -*-
"""Management commands tests."""
from django import test
from django.contrib.auth.models import User
from django.core.management import call_command
from django.core.management.base import CommandError


class CreateUserTestCase(test.TestCase):

    def test_create_user(self):
        call_command('create_cosmos_user', 'usernew', password='s3cret')

        user = User.objects.get(username='usernew')
        self.assertFalse(user.is_superuser)
        self.assertFalse(user.is_staff)

    def test_create_superuser(self):
        call_command('create_cosmos_user', 'usernew', password='s3cret',
                     email=u'usernew@company.com', admin=True)

        user = User.objects.get(username='usernew')
        self.assertTrue(user.is_superuser)
        self.assertTrue(user.is_staff)

    def test_create_user_with_optinal_parameters(self):
        call_command('create_cosmos_user', 'usernew', password='s3cret',
                     email=u'usernew@company.com', first='John', last='Smith')

        user = User.objects.get(username='usernew')
        self.assertEquals(user.email, 'usernew@company.com')
        self.assertEquals(user.first_name, 'John')
        self.assertEquals(user.last_name, 'Smith')
