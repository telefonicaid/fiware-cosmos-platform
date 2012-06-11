"""
Module mongo.py

This module holds functionality to retrieve job results from a mongoDB.
"""
import simplejson as json
from types import ListType, DictType

from django.core.paginator import Paginator, Page
from pymongo import Connection
from pymongo.errors import AutoReconnect, ConnectionFailure

from cosmos import conf
from cosmos.models import JobRun


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
        return self.document.get(self.pk)

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


class MongoPaginator(Paginator):
    """Paginator specialization to optimize mongodb cursors."""

    def __init__(self, cursor, page_size, primary_key):
        Paginator.__init__(self, cursor, page_size)
        self.primary_key = primary_key

    def __len__(self):
        return self.object_list.count()

    def page(self, number):
        "Returns a Page object for the given 1-based page number."
        number = self.validate_number(number)
        bottom = (number - 1) * self.per_page
        records = [MongoRecord(document, self.primary_key) for document in
                   self.object_list.skip(bottom).limit(self.per_page)]
        return Page(records, number, self)


def choose_default_primary_key(collection):
    some_result = collection.find_one()
    if not some_result:
        raise NoResultsError
    return sorted([k for k in some_result.keys() if k not in HIDDEN_KEYS])[0]


def retrieve_results(job_id, primary_key=None):
    ans = []
    job = JobRun.objects.get(pk=job_id)
    try:
        connection = Connection(job.mongo_url())
        try:
            db = connection[job.mongo_db()]
            collection = db[job.mongo_collection()]
            if primary_key is None:
                primary_key = choose_default_primary_key(collection)
            return MongoPaginator(collection.find().sort(primary_key),
                                  conf.RESULTS_PER_PAGE.get(),
                                  primary_key)
        finally:
            connection.close()
    except (AutoReconnect, ConnectionFailure):
        raise NoConnectionError
