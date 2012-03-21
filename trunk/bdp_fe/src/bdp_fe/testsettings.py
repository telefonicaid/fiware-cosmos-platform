"""
Module testsettings

These settings allow Djange unittests to setup a temporary databse and run the
tests of the installed applications
"""

DATABASE_ENGINE = 'sqlite3'
DATABASE_NAME = '/tmp/bdp_fe.db'
INSTALLED_APPS = ['bdp_fe.jobconf']
ROOT_URLCONF = ['bdp_fe.urls']
