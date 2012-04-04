"""
Module mongo.py -

This module holds functionality to retrieve job results from a mongoDB.
"""
import json
from random import choice
from types import ListType, DictType

from pymongo import Connection
from pymongo.errors import AutoReconnect, ConnectionFailure

from bdp_fe.jobconf.models import CustomJobModel, Job

HIDDEN_KEYS = []
EXPAND_TYPES = [ListType, DictType]

class NoResultsError(Exception):
    """
    Raised when there were no records in the queried mongoDB
    """
    pass


class NoConnectionError(Exception):
    """
    Raised when the configured mongoDB is not reachable
    """
    pass


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
        """Gets the values of all non-primary keys for this record. If the
        field is a dictionary or a list, it is encoded as JSON."""
        ans = {}
        for k, v in self.document.iteritems():
            if k != self.pk:
                if type(v) in EXPAND_TYPES:
                    v = json.dumps(v)
                ans.setdefault(k, v)
        return ans

def retrieve_results(job_id, primary_key):
    ans = []
    job = Job.objects.get(id=job_id)
    jobmodel = CustomJobModel.objects.get(job=job)
#     import pdb; pdb.set_trace()
#     jobmodel = JobModel.objects.get(job_id=job_id).customjobmodel
    mongo_url = jobmodel.mongo_url()
    mongo_db = jobmodel.job.user.username
    mongo_collection = 'job_%s' % jobmodel.job.id
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
