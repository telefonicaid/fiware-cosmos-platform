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


def retrieve_results(job_id, primary_key):
    ans = []
    jobmodel = CustomJobModel.objects.get(id=job_id)
    mongo_url = jobmodel.mongo_url()
    mongo_db = jobmodel.job.user.username
    mongo_collection = jobmodel.job.mongo_collection()
    try:
        connection = Connection(mongo_url)
        db = connection[mongo_db]
        job_results = db[mongo_collection]
        if not primary_key:
            some_result = job_results.find_one()
            if not some_result:
                raise NoResultsError
            primary_key = choice([k for k in some_result.keys()
                                 if k not in HIDDEN_KEYS])
        for job_result in job_results.find():
            mongo_result = MongoRecord(job_result, primary_key)
            ans.append(mongo_result)
        return ans
    except (AutoReconnect, ConnectionFailure):
        raise NoConnectionError


def cluster_connection():
    return settings.CLUSTER_CONF['factory'](settings.CLUSTER_CONF['host'],
                                            settings.CLUSTER_CONF['port'])
