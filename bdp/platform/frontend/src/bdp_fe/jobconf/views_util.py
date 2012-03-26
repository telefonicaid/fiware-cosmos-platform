"""
Utility functions for controllers.

"""
from django.http import HttpResponseNotFound
from django.shortcuts import get_object_or_404
from pymongo import Connection

from bdp_fe.middleware403 import Http403
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
    ## TODO: make configurable
    connection = Connection('localhost', 27017)
    db = connection.test_database
    job_results = db.test_collection
    for job_result in job_results.find({"job_id" : job_id}):
        mongo_result = MongoRecord(job_result, primary_key)
        ans.append(mongo_result)
    return ans
