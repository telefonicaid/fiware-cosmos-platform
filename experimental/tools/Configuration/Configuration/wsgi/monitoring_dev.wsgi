import os
import sys

sys.path.append('/Users/def/tid/BigData/workspace/Configuration/Configuration/settings')

os.environ['DJANGO_SETTINGS_MODULE'] = 'settings_dev'

import django.core.handlers.wsgi
application = django.core.handlers.wsgi.WSGIHandler()
