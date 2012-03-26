"""
Utility functions for controllers.

"""
from pymongo import Connection

def safe_int_param(query_dict, param_name, default_value=None):
    """
    Safe conversion of query parameters to int.

    By default, returns None for absent or non-integer values.
    """
    try:
        return int(query_dict.get(param_name, ''))
    except ValueError:
        return default_value

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
