"""
Handling 403's with exceptions.

"""

from django.conf import settings
from django.core.exceptions import PermissionDenied
from django.http import HttpResponseForbidden
from django.template import RequestContext, loader

class Http403(Exception):
    pass

def render_to_403(*args, **kwargs):
    """
    Creates a 403 response page.
    
    """
    if not isinstance(args, list):
        args = []
    args.append('403.html')
    http_response_kwargs = {
        'mimetype': kwargs.pop('mimetype', None)
    }
    return HttpResponseForbidden(loader.render_to_string(*args, **kwargs),
                                 **http_response_kwargs)

class Http403Middleware(object):

    def process_exception(self, request, exception):
        if isinstance(exception, Http403):
            if settings.DEBUG:
                raise PermissionDenied
            else:
                return render_to_403(context_instance=RequestContext(request))
