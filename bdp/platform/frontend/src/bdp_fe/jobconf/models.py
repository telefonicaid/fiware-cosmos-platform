"""
Data models.

"""

from django.contrib.auth.models import User
from django.db import models

class Job(models.Model):
    """
    A Job is a calculation to be run on the BDP
    """
    CREATED = 0
    RUNNING = 1
    SUCESSFUL = 2
    FAILED = 3

    JOBSTATUS_CHOICES = (
        (CREATED, 'Created'),
        (RUNNING, 'Running'),
        (SUCESSFUL, 'Successful'),
        (FAILED, 'Failed'),
    )

    name = models.CharField(max_length=40)
    user = models.ForeignKey(User)
    date = models.DateTimeField(auto_now=True)
    status = models.IntegerField(choices=JOBSTATUS_CHOICES)
    execution_id = models.CharField(max_length=256, null=True, blank=True)

# TODO: expand to a hierarchy of models
class JobModel(models.Model):
    """
    Job model represents the calculation to be performed.
    """
    job = models.OneToOneField(Job)
    jar_name = models.CharField(max_length=256, null=True, blank=True)
    
