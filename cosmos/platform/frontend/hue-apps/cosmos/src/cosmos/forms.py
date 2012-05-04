"""
Application forms and validation.

"""
from django import forms

from cosmos.models import Dataset


class UploadDatasetForm(forms.Form):
    name = forms.CharField(max_length=Dataset.NAME_MAX_LENGTH, label='Name')
    description = forms.CharField(max_length=Dataset.DESCRIPTION_MAX_LENGTH,
                                  widget=forms.Textarea, label='Description')
    # The "hdfs_" prefix triggers the HDFSfileUploadHandler
    hdfs_file = forms.FileField(label='File to upload')
