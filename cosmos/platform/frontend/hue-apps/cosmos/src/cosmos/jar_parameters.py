# -*- coding: utf-8 -*-
"""
JAR parameters representation.
"""
from django import forms
from django.core.exceptions import ValidationError

from cosmos.expansion import ExpansionContext
from cosmos.forms import ABSOLUTE_PATH_VALIDATOR, ID_VALIDATOR, HDFSFileChooser
from cosmos.mongo import user_coll_url


class AbstractParameter(object):
    """Base class for custom JAR parameters.

    Do not use it directly, only through subclassing.
    """

    def __init__(self, name, default_value=None, expansion=ExpansionContext()):
        self.name = name
        self.validate(default_value, expansion)
        self.default_value = default_value
        self.__value = None

    def set_value(self, value, expansion):
        self.validate(value, expansion)
        self.__value = value

    def get_value(self):
        if self.has_value():
            return self.__value
        else:
            return self.default_value

    def has_value(self):
        return self.__value is not None

    def validate(self, value, expansion):
        """Throw ValueError if the value is not acceptable for the parameter.

        To be overridden by subclasses.
        """
        raise NotImplementedError(
            "AbstractParameter#validate must be overridden")

    def form_field(self, expansion):
        """Generate a field suitable for a django form.

        To be overridden by subclasses.
        """
        raise NotImplementedError(
            "AbstractParameter#form_field must be overridden")

    def as_job_argument(self, job, expansion):
        """Returns a list of command line arguments to inject this
        parameter into a job.

        Variables on the paramters are expanded by means of expansion object.

        Can be overriden by subclasses.
        """
        return ["-D", "%s=%s" % (self.name, expansion.expand(self.get_value()))]


class StringParameter(AbstractParameter):
    MAX_LENGTH = 255

    def validate(self, value, expansion):
        if value is None:
            return
        if not isinstance(value, basestring):
            raise ValueError('Found %s when string was expected' % type(value))
        if len(value) > self.MAX_LENGTH:
            raise ValueError('Max length of %d was exceed: %d characters' %
                             (self.MAX_LENGTH, len(value)))

    def form_field(self, expansion):
        return forms.CharField(label=self.name,
                               max_length=self.MAX_LENGTH,
                               initial=self.default_value)


class FilePathParameter(StringParameter):

    def validate(self, value, expansion):
        super(FilePathParameter, self).validate(value, expansion)
        if value is None:
            return
        validator = expansion.decorate(ABSOLUTE_PATH_VALIDATOR)
        try:
            validator(value)
        except ValidationError, e:
            raise ValueError('Invalid absolute path: "%s"' % value)

    def form_field(self, expansion):
        validator = expansion.decorate(ABSOLUTE_PATH_VALIDATOR)
        return forms.CharField(label=self.name,
                               max_length=StringParameter.MAX_LENGTH,
                               initial=self.default_value,
                               widget=HDFSFileChooser(),
                               validators=[validator])


class MongoCollParameter(StringParameter):

    def __init__(self, name, default_value=None, expansion=ExpansionContext()):
        if default_value is None:
            default_value = "job_${job.id}"
        super(MongoCollParameter, self).__init__(name, expansion=expansion,
                                                 default_value=default_value)

    def validate(self, value, expansion):
        super(MongoCollParameter, self).validate(value, expansion)
        if value is None:
            return
        validator = expansion.decorate(ID_VALIDATOR)
        try:
            validator(value)
        except ValidationError:
            raise ValueError(('Invalid MongoDB collection name: "%s" '
                              '(only letters, numbers and dashes)') % value)

    def form_field(self, expansion):
        return forms.CharField(label=self.name,
                               max_length=StringParameter.MAX_LENGTH,
                               initial=self.default_value,
                               validators=[expansion.decorate(ID_VALIDATOR)])

    def as_job_argument(self, job, expansion):
        collection = expansion.expand(self.get_value())
        self.validate(collection, expansion)
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
