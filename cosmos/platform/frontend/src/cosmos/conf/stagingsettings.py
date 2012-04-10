"""
Module statingsettings

"""

DEBUG = False
TEMPLATE_DEBUG = DEBUG

SECRET_KEY = ')qif7^e0_0@sx!o0ca$c6v8%mz+y2$r0liegqd8(2k1^4reihj'

from cosmos.conf.base_settings import *

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

LANDING_ROOT = '/var/cosmos/landing/'
STATIC_ROOT = 'static'

FCGI_OPTIONS = {
    'protocol': 'fcgi',
    'host': '127.0.0.1',
    'method': 'threaded',
    'port': '9000',
    'pidfile': '/var/run/cosmos/django.pid',
}

CLUSTER_CONF = {
    'connection-factory': 'cosmos.jobconf.cluster.remote.Cluster',
    'host': '127.0.0.1',
    'port': 9888,
    'mongobase': 'mongodb://10.173.128.148',
}

LOGGING = {
    'version': 1,
    'disable_existing_loggers': False,
    'handlers': {
        'mail_admins': {
            'level': 'ERROR',
            'class': 'django.utils.log.AdminEmailHandler'
        },
        'logfile': {
            'level': 'DEBUG',
            'class': 'logging.FileHandler',
            'formatter': 'detailed',
            'filename': '/var/log/cosmos/cosmos-frontend.log',
        },
        'console': {
            'level': 'DEBUG',
            'class': 'logging.StreamHandler',
            'formatter': 'simple'
        },
        'null': {
            'level': 'DEBUG',
            'class': 'django.utils.log.NullHandler',
        }
    },
    'formatters': {
        'verbose': {
            'format': '%(levelname)s %(asctime)s %(module)s %(message)s'
        },
        'detailed': {
            'format': '%(levelname)s %(asctime)s %(module)s:%(lineno)d: ' +
                      '%(message)s',
        },
        'simple': {
            'format': '%(levelname)s %(message)s'
        },
    },
    'loggers': {
        'django': {
            'handlers': ['console', 'console', 'mail_admins'],
            'level': 'ERROR',
            'propagate': True,
        },
        'bdp_fe': {
            'handlers': ['console', ''logfile, 'mail_admins'],
            'level': 'DEBUG',
            'propagate': True,
        }
    }
}
