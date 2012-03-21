# -*- coding: utf-8 -*-
"""Main configuration"""

from os import path
PROJECT_PATH = path.dirname(__file__)

TEMPLATE_DIRS = (
    # Don't forget to use absolute paths, not relative paths.
    path.join(PROJECT_PATH, "templates"),
)

INSTALLED_APPS = (
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.sites',
    'django.contrib.messages',
    'django.contrib.staticfiles',
    'django.contrib.admin',
    'jobconf',
)

LOGIN_URL = '/accounts/login/'
