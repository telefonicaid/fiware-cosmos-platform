"""
Custom middleware

"""

from django.conf import settings
from django.core.exceptions import PermissionDenied
from bdp_fe.http import Http403

class Http403Middleware(object):

    def process_exception(self, request, exception):
        if isinstance(exception, Http403):
            if settings.DEBUG:
                raise PermissionDenied
            else:
                return render_to_403(context_instance=RequestContext(request))
