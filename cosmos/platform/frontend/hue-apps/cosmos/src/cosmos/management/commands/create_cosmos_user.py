# -*- coding: utf-8 -*-
"""Create user command."""
from django.core.management.base import BaseCommand, CommandError

class Command(BaseCommand):
    args = 'login password [email] [full name]'
    help = 'Register a new user to the cosmos platform'

    def handle(self, *args, **options):
        self.stdout.write('Foo %s, %s' % (args, options))
