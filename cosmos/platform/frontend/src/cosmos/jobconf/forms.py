"""
Application forms and validation.

"""
from django import forms

from cosmos.jobconf.models import Job


class NewJobForm(forms.Form):
    name = forms.CharField(max_length=Job.NAME_MAX_LENGTH)


class UploadJarForm(forms.Form):
    file = forms.FileField()


class UploadDataForm(forms.Form):
    file = forms.FileField()
