"""
Module testsettings

These settings allow Django unittests to setup a temporary databse and run the
tests of the installed applications.

"""

DEBUG = True
TEMPLATE_DEBUG = DEBUG

from settings import *

DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.sqlite3',
        'NAME': '/tmp/bdp_fe.db'
    }
}

MEDIA_ROOT = ''
MEDIA_URL = ''
STATIC_ROOT = ''
STATIC_URL = '/static/'
ADMIN_MEDIA_PREFIX = '/static/admin/'

ADMINS = (
    ('admin', 'cosmos-prod@tid.es'),
)
MANAGERS = ADMINS

LANDING_ROOT = '/tmp/landing/'

CLUSTER_CONF = {
    'host': 'localhost',
    'port': 9888,
    'mongobase': 'mongodb://pshdp04',
}
