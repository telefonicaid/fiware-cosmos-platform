# -*- coding: utf-8 -*-
"""
JAR parameters representation.
"""
from django import forms
from django.core.exceptions import ValidationError
from mako.template import Template

from cosmos.forms import ABSOLUTE_PATH_VALIDATOR, ID_VALIDATOR, HDFSFileChooser
from cosmos.mongo import user_coll_url


class AbstractParameter(object):
    """Base class for custom JAR parameters.

    Do not use it directly, only through subclassing.
    """

    def __init__(self, name, default_value=None):
        self.name = name
        self.validate(default_value)
        self.default_value = default_value
        self.__value = None

    def set_value(self, value):
        self.validate(value)
        self.__value = value

    def get_value(self):
        if self.has_value():
            return self.__value
        else:
            return self.default_value

    def has_value(self):
        return self.__value is not None

    def validate(self, value):
        """Throw ValueError if the value is not acceptable for the parameter.

        To be overridden by subclasses.
        """
        raise NotImplementedError(
            "AbstractParameter#validate must be overridden")

    def form_field(self):
        """Generate a field suitable for a django form.

        To be overridden by subclasses.
        """
        raise NotImplementedError(
            "AbstractParameter#form_field must be overridden")

    def as_job_argument(self, job, context={}):
        """Returns a list of command line arguments to inject this
        parameter into a job.

        Context is a dictionary with the variables subject to expansion.

        Can be overriden by subclasses.
        """
        template = Template(self.get_value())
        return ["-D", "%s=%s" % (self.name, template.render(**context))]


class StringParameter(AbstractParameter):
    MAX_LENGTH = 255

    def validate(self, value):
        if value is None:
            return
        if type(value) != str:
            raise ValueError('Found %s when string was expected' % type(value))
        if len(value) > self.MAX_LENGTH:
            raise ValueError('Max length of %d was exceed: %d characters' %
                             (self.MAX_LENGTH, len(value)))

    def form_field(self):
        return forms.CharField(label=self.name,
                               max_length=self.MAX_LENGTH,
                               initial=self.default_value)


class FilePathParameter(StringParameter):

    def validate(self, value):
        super(FilePathParameter, self).validate(value)
        if value is None:
            return
        try:
            ABSOLUTE_PATH_VALIDATOR(value)
        except ValidationError, e:
            raise ValueError('Invalid absolute path: "%s"' % value)

    def form_field(self):
        return forms.CharField(label=self.name,
                               max_length=StringParameter.MAX_LENGTH,
                               initial=self.default_value,
                               widget=HDFSFileChooser(),
                               validators=[ABSOLUTE_PATH_VALIDATOR])


class MongoCollParameter(StringParameter):

    def __init__(self, name, default_value=None):
        if default_value is None:
            default_value = "job_${job['id']}"
        super(MongoCollParameter, self).__init__(
            name, default_value=default_value)

    def validate(self, value):
        super(MongoCollParameter, self).validate(value)
        if value is None:
            return
        try:
            ID_VALIDATOR(value)
        except ValidationError, e:
            raise ValueError(('Invalid MongoDB collection name: "%s" '
                              '(only letters, numbers and dashes)') % value)

    def form_field(self):
        return forms.CharField(label=self.name,
                               max_length=StringParameter.MAX_LENGTH,
                               initial=self.default_value,
                               validators=[ID_VALIDATOR])

    def as_job_argument(self, job, context={}):
        template = Template(self.get_value())
        collection = template.render(**context)
        self.validate(collection)
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
