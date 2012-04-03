"""
HTTP utils.

"""
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
    response = HttpResponseForbidden(loader.render_to_string(*args, **kwargs),
                                     **http_response_kwargs)
