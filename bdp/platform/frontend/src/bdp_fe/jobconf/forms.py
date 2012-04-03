"""
Application forms and validation.

"""
import logging

from django import forms

from bdp_fe.jobconf.models import Job


class NewJobForm(forms.Form):
    name = forms.CharField(max_length=Job.NAME_MAX_LENGTH)


class UploadJarForm(forms.Form):
    file = forms.FileField()


class UploadDataForm(forms.Form):
    file = forms.FileField()
