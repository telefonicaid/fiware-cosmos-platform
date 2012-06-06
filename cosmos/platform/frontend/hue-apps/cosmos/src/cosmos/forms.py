"""
Application forms and validation.
"""
import re

from django import forms
from django.core import validators
from django.forms.util import ErrorList
from django.utils.safestring import mark_safe

from cosmos import models


ID_VALIDATOR = validators.RegexValidator(
    regex=re.compile(r'^[a-z0-9\-_]+$', re.IGNORECASE),
    message='Enter a valid identifier (only letters, numbers and dashes)')


ABSOLUTE_PATH_VALIDATOR = validators.RegexValidator(
    regex=re.compile(r'^(/\.?[^./][^/]*)+/?$'),
    message='Enter a valid absolute path')


JAR_FILE_VALIDATOR = validators.RegexValidator(
    regex=re.compile(r'\.jar$', re.IGNORECASE),
    message='Filename must have "jar" extension')


class DefineJobForm(forms.Form):
    name = forms.CharField(max_length=models.JobRun.NAME_MAX_LENGTH,
                           label='Name', validators=[ID_VALIDATOR])
    description = forms.CharField(
        max_length=models.JobRun.DESCRIPTION_MAX_LENGTH,
        label='Description', required=False)
    jar_path = forms.CharField(max_length=models.PATH_MAX_LENGTH,
                               validators=[ABSOLUTE_PATH_VALIDATOR,
                                           JAR_FILE_VALIDATOR],
                               label='JAR file')
    dataset_path = forms.CharField(max_length=models.PATH_MAX_LENGTH,
                                   validators=[ABSOLUTE_PATH_VALIDATOR],
                                   label='Dataset path')

    def is_valid(self, fs):
        valid = super(forms.Form, self).is_valid()
        if not valid:
            return valid
        has_jar = self.__validate_hdfs_path(fs, 'jar_path')
        has_dataset = self.__validate_hdfs_path(fs, 'dataset_path')
        return has_jar and has_dataset

    def __validate_hdfs_path(self, fs, field):
        path = self.cleaned_data[field]
        has_file = fs.exists(path)
        if not has_file:
            errors = self._errors.setdefault(field, ErrorList())
            errors.append('File "%s" does not exist' % path)
        return has_file


class HDFSFileChooser(forms.TextInput):
    FORMAT_STRING = ('%s <a class="hue-choose_file" data-filters="ArtButton" ' +
                     'data-chooseFor="%s">...</a>')

    def render(self, name, value, attrs=None):
        html = super(HDFSFileChooser, self).render(name, value, attrs=attrs)
        return mark_safe(self.FORMAT_STRING % (html, name))


class ParameterizeJobForm(forms.Form):
    """
    Job parametrization form whose fields are dynamically generated from
    parameter descriptions.
    """

    STRING_MAX_LENGTH = 255

    FIELD_FACTORIES = {
        'string': (lambda template:
                       forms.CharField(label=template['name'],
                                       max_length=ParameterizeJobForm.
                                                  STRING_MAX_LENGTH,
                                       initial=template.get('default_value',
                                                            None))),
        'filepath': (lambda template:
                       forms.CharField(label=template['name'],
                                       max_length=ParameterizeJobForm.
                                                  STRING_MAX_LENGTH,
                                       initial=template.get('default_value',
                                                            None),
                                       widget=HDFSFileChooser(),
                                       validators=[ABSOLUTE_PATH_VALIDATOR]))
    }

    def __init__(self, parameters, data=None):
        """
        Parameters is a list of dictionaries suppoting these keywords:
         - name
         - type
         - default_value (optional)
        """
        if data is not None:
            # Init from request data
            super(ParameterizeJobForm, self).__init__(data)
        else:
            super(ParameterizeJobForm, self).__init__()

        for param in parameters:
            self.fields[param['name']] = self.FIELD_FACTORIES[param['type']](param)
