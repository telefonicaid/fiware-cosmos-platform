# -*- coding: utf-8 -*-
"""Create user command."""
from optparse import make_option
import subprocess

from django.contrib.auth.models import User
from django.core.management.base import BaseCommand, CommandError
from hadoop import cluster


ADD_USER_COMMAND = '/usr/sbin/useradd'
DEFAULT_USER_GROUP = 'nobody'
DEFAULT_USER_SHELL = '/sbin/nologin'


class Command(BaseCommand):
    args = 'login'
    help = 'Register a new user to the cosmos platform'
    option_list = BaseCommand.option_list + (
        make_option('--password', action='store',
                    help='User password'),
        make_option('--email', action='store',
                    default='', help='User email address'),
        make_option('--first', action='store',
                    default='', help='First name'),
        make_option('--last', action='store',
                    default='', help='Family name'),
        make_option('--admin', action='store_true', default=False,
                    help='Grant administrative privileges')
    )

    def handle(self, *args, **options):
        if len(args) != 1:
            raise CommandError("A single login was expected")
        login = args[0]
        if options.get('password', None) is None:
            raise CommandError("Please specify a password with --password")
        self.create_django_user(login, options)
        self.create_hdfs_home(login)
        self.create_unix_user(login)

    def create_django_user(self, login, options):
        try:
            user = User.objects.get(username=login)
            self.stderr.write(('Warning: django user %s already exists, '
                               'updating attributes\n') % login)
        except User.DoesNotExist:
            user = User.objects.create_user(login, options['email'],
                                            options['password'])
        user.first_name = options['first']
        user.last_name = options['last']
        admin = options.get('admin', False)
        user.is_superuser = admin
        user.is_staff = admin
        user.save()

    def create_hdfs_home(self, login):
        home = '/user/%s' % login
        hdfs = cluster.get_hdfs()
        hdfs.setuser(login)
        for path in ['jars', 'datasets', 'tmp']:
            hdfs.mkdir('%s/%s' % (home, path))

    def create_unix_user(self, login):
        try:
            ret_code = subprocess.call([ADD_USER_COMMAND, login,
                                        '-g', DEFAULT_USER_GROUP,
                                        '-s', DEFAULT_USER_SHELL])
        except OSError, e:
            raise CommandError('Cannot create unix account %s' % login, e)

        if ret_code == 9:
            self.stderr.write('Warning: unix account %s already exists\n' %
                              login)
        elif ret_code != 0:
            raise CommandError(('Unexpected error (recode %d) when creating '
                                'unix account for %s') % login)
