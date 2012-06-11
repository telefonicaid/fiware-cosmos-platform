"""
Application forms and validation.
"""
import re

from django import forms
from django.core import validators
from django.forms.util import ErrorList
from django.utils.safestring import mark_safe

from cosmos import conf
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


class HDFSFileChooser(forms.TextInput):
    FORMAT_STRING = ('%s <a class="hue-choose_file" data-filters="ArtButton" ' +
                     'data-chooseFor="%s">...</a>')

    def render(self, name, value, attrs=None):
        html = super(HDFSFileChooser, self).render(name, value, attrs=attrs)
        return mark_safe(self.FORMAT_STRING % (html, name))


def validate_hdfs_path(form, field, fs):
    path = form.cleaned_data[field]
    has_file = fs.exists(path)
    if not has_file:
        errors = form._errors.setdefault(field, ErrorList())
        errors.append('File "%s" does not exist' % path)
    return has_file


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

    def is_valid(self, fs):
        return (super(forms.Form, self).is_valid() and
                validate_hdfs_path(self, 'jar_path', fs))


class BasicConfigurationForm(forms.Form):
    dataset_path = forms.CharField(max_length=models.PATH_MAX_LENGTH,
                                   validators=[ABSOLUTE_PATH_VALIDATOR],
                                   widget=HDFSFileChooser(),
                                   label='Dataset path')

    def is_valid(self, fs):
        return (super(forms.Form, self).is_valid() and
                validate_hdfs_path(self, 'dataset_path', fs))


class ParameterizeJobForm(forms.Form):
    """
    Job parametrization form whose fields are dynamically generated from
    parameter descriptions.
    """

    def __init__(self, parameters):
        """
        Parameters is a list of JAR parameters to be rendered as a form.
        """
        if any(param.has_value() for param in parameters):
            data = dict([(param.name, param.get_value()) for param in
                         parameters])
        else:
            data = None
        super(ParameterizeJobForm, self).__init__(data)

        for param in parameters:
            self.fields[param.name] = param.form_field()
