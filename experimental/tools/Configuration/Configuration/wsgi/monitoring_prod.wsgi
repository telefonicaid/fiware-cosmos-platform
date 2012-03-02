import os
import sys

sys.path.append('/usr/local/ps/apps/bdp/Configuration/Configuration/settings')

os.environ['DJANGO_SETTINGS_MODULE'] = 'Configuration.settings'

import django.core.handlers.wsgi
application = django.core.handlers.wsgi.WSGIHandler()
