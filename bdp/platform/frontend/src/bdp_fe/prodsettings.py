"""
Module prodsettings

These settings allow Django unittests to setup a temporary databse and run the
tests of the installed applications.

"""

DEBUG = True
TEMPLATE_DEBUG = DEBUG

SECRET_KEY = ')qif7^e0_0@sx!o0ca$c6v8%mz+y2$r0liegqd8(2k1^4reihj'

from settings import *

DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.mysql',
        'NAME': 'bdp_fe',
        'USER': 'bdp',
        'PASSWORD': 'XFs4dm8J74e82oO',
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

FCGI_OPTIONS = {
    'protocol': 'fcgi',
    'host': '127.0.0.1',
    'method': 'threaded',
    'port': '9000',
    'pidfile': '/var/run/bdp_fe/django.pid',
}
