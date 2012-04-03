# -*- coding: utf-8 -*-
"""Main configuration"""

from os import path
PROJECT_PATH = path.abspath(path.join(path.dirname(__file__), '..'))

SITE_ID = 1

ROOT_URLCONF = 'bdp_fe.urls'

TEMPLATE_DIRS = (
    # Don't forget to use absolute paths, not relative paths.
    path.join(PROJECT_PATH, "templates"),
)
TEMPLATE_LOADERS = (
    'django.template.loaders.filesystem.Loader',
    'django.template.loaders.app_directories.Loader',
#     'django.template.loaders.eggs.Loader',
)
TEMPLATE_CONTEXT_PROCESSORS = (
    'django.core.context_processors.debug',
    'django.core.context_processors.i18n',
    'django.core.context_processors.media',
    'django.core.context_processors.static',
    'django.contrib.auth.context_processors.auth',
    'django.contrib.messages.context_processors.messages',
)

FIXTURE_DIRS = (
    path.join(PROJECT_PATH, "fixtures"),
)

STATICFILES_FINDERS = (
    'django.contrib.staticfiles.finders.FileSystemFinder',
    'django.contrib.staticfiles.finders.AppDirectoriesFinder',
#    'django.contrib.staticfiles.finders.DefaultStorageFinder',
)

MIDDLEWARE_CLASSES = (
    'django.middleware.common.CommonMiddleware',
    'django.contrib.sessions.middleware.SessionMiddleware',
    'django.middleware.csrf.CsrfViewMiddleware',
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    'django.contrib.messages.middleware.MessageMiddleware',
    'bdp_fe.middleware.Http403Middleware',
)

INSTALLED_APPS = (
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.sites',
    'django.contrib.messages',
    'django.contrib.staticfiles',
    'django.contrib.admin',
    'bdp_fe.jobconf',
)

LOGIN_URL = '/accounts/login/'

# UI settings
MIN_RELOAD_PERIOD = 1
RELOAD_PERIOD = 30
