"""
Utility functions for controllers.

"""
from django.http import HttpResponseNotFound
from django.shortcuts import get_object_or_404

from bdp_fe.http import Http403

from bdp_fe.jobconf.models import Job

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
