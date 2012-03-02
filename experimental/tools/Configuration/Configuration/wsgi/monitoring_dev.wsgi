import os
import sys

sys.path.append('/Users/def/tid/BigData/workspace/Configuration/Configuration')

os.environ['DJANGO_SETTINGS_MODULE'] = 'settings.dev'

import django.core.handlers.wsgi
application = django.core.handlers.wsgi.WSGIHandler()
