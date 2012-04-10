"""
Utility functions for controllers.

"""
import json
from random import choice
from types import ListType, DictType

from django.conf import settings
from django.http import HttpResponseNotFound
from django.shortcuts import get_object_or_404

from cosmos.http import Http403
from cosmos.jobconf.models import Job


def safe_int_param(query_dict, param_name, default_value=None):
    """
    Safe conversion of query parameters to int.

    By default, returns None for absent or non-integer values.
    """
    try:
        return int(query_dict.get(param_name, ''))
    except ValueError:
        return default_value


def get_owned_job_or_40x(request, job_id):
    try:
        job = get_object_or_404(Job, pk=int(job_id))
    except ValueError:
        raise HttpResponseNotFound()
    if job.user == request.user:
        return job
    else:
        raise Http403()


def get_class( class_name ):
    """Lookup a class by fully qualified name"""
    parts = class_name.split('.')
    module = ".".join(parts[:-1])
    m = __import__( module )
    for comp in parts[1:]:
        m = getattr(m, comp)
    return m


def cluster_connection():
    factory = get_class(settings.CLUSTER_CONF['connection-factory'])
    return factory(settings.CLUSTER_CONF['host'],
                   settings.CLUSTER_CONF['port'])
