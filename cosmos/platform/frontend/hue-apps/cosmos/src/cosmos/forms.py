"""
Application forms and validation.

"""
import re

from django import forms
from django.core import validators

from cosmos.models import Dataset, CustomJar

ID_VALIDATOR = validators.RegexValidator(
    regex=re.compile(r'^[a-z][a-z0-9-_]*$', re.IGNORECASE),
    message='Enter a valid identifier (only letters, numbers and dashes' +
            'starting by a number)')

class UploadDatasetForm(forms.Form):
    name = forms.CharField(max_length=Dataset.NAME_MAX_LENGTH, label='Name',
                           validators=[ID_VALIDATOR])
    description = forms.CharField(max_length=Dataset.DESCRIPTION_MAX_LENGTH,
                                  label='Description')
    # The "hdfs_" prefix triggers the HDFSfileUploadHandler
    hdfs_file = forms.FileField(label='File to upload')


class UploadCustomJarForm(forms.Form):
    name = forms.CharField(max_length=CustomJar.NAME_MAX_LENGTH, label='Name',
                           validators=[ID_VALIDATOR])
    description = forms.CharField(max_length=CustomJar.DESCRIPTION_MAX_LENGTH,
                                  label='Description')
    # The "hdfs_" prefix triggers the HDFSfileUploadHandler
    hdfs_file = forms.FileField(label='File to upload')
