# -*- coding: utf-8 -*-
"""Management commands tests."""
import os
import os.path
from tempfile import mkdtemp
from shutil import rmtree

from django import test
from django.contrib.auth.models import User
from django.core.management import call_command

from cosmos.management.commands import create_cosmos_user


class CreateUserTestCase(test.TestCase):

    def setUp(self):
        self.temp_dir = mkdtemp()
        print("Temporal dir %s" % self.temp_dir)
        self.fake_add_user_cmd = os.path.join(self.temp_dir, 'adduser')
        self.args_file = os.path.join(self.temp_dir, 'args')
        f = open(self.fake_add_user_cmd, 'w')
        f.write("""#!/bin/bash
echo $* > %s
""" % self.args_file)
        f.close()
        os.chmod(self.fake_add_user_cmd, 0555)

        self.add_user_cmd = create_cosmos_user.ADD_USER_COMMAND
        create_cosmos_user.ADD_USER_COMMAND = self.fake_add_user_cmd

    def tearDown(self):
        rmtree(self.temp_dir)
        create_cosmos_user.ADD_USER_COMMAND = self.add_user_cmd

    def test_create_unix_user(self):
        call_command('create_cosmos_user', 'usernew', password='s3cret')

        user = User.objects.get(username='usernew')
        self.assertFalse(user.is_superuser)
        self.assertFalse(user.is_staff)

        # 'useradd' is not called to not to alter the system during testing.
        # However, command arguments are saved and asserted upon.
        args_file = open(self.args_file, 'r')
        args = args_file.read()
        args_file.close()
        self.assertEquals(args, "usernew -g nobody -s /sbin/nologin\n")

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
