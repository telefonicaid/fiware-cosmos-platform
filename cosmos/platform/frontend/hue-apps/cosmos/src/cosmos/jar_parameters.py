# -*- coding: utf-8 -*-
"""
JAR parameters representation.
"""
from django import forms

from cosmos.forms import ABSOLUTE_PATH_VALIDATOR, ID_VALIDATOR, HDFSFileChooser
from cosmos.mongo import user_coll_url


class AbstractParameter(object):
    """Base class for custom JAR parameters.

    Do not use it directly, only through subclassing.
    """

    def __init__(self, name, default_value=None):
        self.name = name
        self.default_value = default_value
        self.__value = None

    def set_value(self, value):
        self.__value = value

    def get_value(self):
        if self.has_value():
            return self.__value
        else:
            return self.default_value

    def has_value(self):
        return self.__value is not None

    def form_field(self):
        """Generate a field suitable for a django form.

        To be override by subclasses.
        """
        raise NotImplementedError("AbstractParameter#form_field must be override")

    def as_job_argument(self, job):
        """Returns a list of command line arguments to inject this
        parameter into a job.

        Can be overriden by subclasses.
        """
        return ["-D", "%s=%s" % (self.name, self.get_value())]


class StringParameter(AbstractParameter):
    MAX_LENGTH = 255

    def form_field(self):
        return forms.CharField(label=self.name,
                               max_length=self.MAX_LENGTH,
                               initial=self.default_value)


class FilePathParameter(AbstractParameter):

    def form_field(self):
        return forms.CharField(label=self.name,
                               max_length=StringParameter.MAX_LENGTH,
                               initial=self.default_value,
                               widget=HDFSFileChooser(),
                               validators=[ABSOLUTE_PATH_VALIDATOR])


class MongoCollParameter(AbstractParameter):

    def form_field(self):
        return forms.CharField(label=self.name,
                               max_length=StringParameter.MAX_LENGTH,
                               initial=self.default_value,
                               validators=[ID_VALIDATOR])

    def as_job_argument(self, job):
        collection = self.get_value()
        if collection is None:
            # Fallback to default collection name
            collection = job.mongo_collection()
        mongo_url = user_coll_url(job.user.id, collection)
        return ["-D", "%s=%s" % (self.name, mongo_url)]


PARAMETER_MAP = {'string': StringParameter,
                 'filepath': FilePathParameter,
                 'mongocoll': MongoCollParameter}


def make_parameter(name, value):
    if name.find('=') >= 0:
        raise ValueError("invalid parameter name '%s'" % name)
    tokens = value.split('|', 1)
    type_name = tokens[0]
    if not PARAMETER_MAP.has_key(type_name):
        raise ValueError('unknown type "%s"' % type_name)
    if len(tokens) > 1:
        return PARAMETER_MAP[type_name](name, default_value=tokens[1])
    else:
        return PARAMETER_MAP[type_name](name)
