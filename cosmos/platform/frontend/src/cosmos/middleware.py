"""
Custom middleware

"""

from django.conf import settings
from django.core.exceptions import PermissionDenied
from django.template import RequestContext

from cosmos.http import Http403, render_to_403


class Http403Middleware(object):
    """Renders Http403 exceptions as error pages"""

    def process_exception(self, request, exception):
        if isinstance(exception, Http403):
            if settings.DEBUG:
                raise PermissionDenied
            else:
                return render_to_403(context_instance=RequestContext(request))
