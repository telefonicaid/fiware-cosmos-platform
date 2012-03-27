"""
Utility functions for controllers.

"""
from django.conf import settings
from django.http import HttpResponseNotFound
from django.shortcuts import get_object_or_404
from models import CustomJobModel
from pymongo import Connection
from random import choice

from bdp_fe.jobconf.models import Job
from bdp_fe.middleware403 import Http403

HIDDEN_KEYS = ['_id', 'job_id']

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
    

class MongoRecord(object):
    """
    A MongoRecord is a document from a Mongo database, but with additional
    methods to allow for easier display.
    """
    def __init__(self, raw_mongo_document, primary_key):
        self.document = raw_mongo_document
        self.pk = primary_key

    def get_primary_key(self):
        """Gets the value of the primary key for this record"""
        return self.document[self.pk]

    def get_fields(self):
        """Gets the values of all non-primary keys for this record"""
        ans = {}
        for k, v in self.document.iteritems():
            if k != self.pk:
                ans.setdefault(k, v)
        return ans

def retrieve_results(job_id, primary_key):
    ans = []
    jobmodel = CustomJobModel.objects.get(id=job_id)
    mongo_url = jobmodel.mongo_url()
    mongo_db = jobmodel.job.user.username
    mongo_collection = 'job_%s' % jobmodel.job.id
    connection = Connection(mongo_url)
    db = connection[mongo_db]
    job_results = db[mongo_collection]
    if not primary_key:
        some_result = job_results.find_one()
        primary_key = choice([k for k in some_result.keys()
                             if k not in HIDDEN_KEYS])
    for job_result in job_results.find():
        mongo_result = MongoRecord(job_result, primary_key)
        ans.append(mongo_result)
    return ans
