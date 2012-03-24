"""
Data models.

"""

from django.contrib.auth.models import User
from django.db import models

class Job(models.Model):
    """
    A Job is a calculation to be run on the BDP
    """

    NAME_MAX_LENGTH = 40
    name = models.CharField(max_length=NAME_MAX_LENGTH)

    user = models.ForeignKey(User)

    date = models.DateTimeField(auto_now=True)

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
    status = models.IntegerField(choices=JOBSTATUS_CHOICES)

    EXECUTION_ID_MAX_LENGTH = 256
    execution_id = models.CharField(null=True, blank=True,
        max_length=EXECUTION_ID_MAX_LENGTH)

    def __unicode__(self):
        return self.name

# TODO: expand to a hierarchy of models
class JobModel(models.Model):
    """
    Job model represents the calculation to be performed.
    """
    job = models.OneToOneField(Job)
    jar_name = models.CharField(max_length=256, null=True, blank=True)
    
    def __unicode__(self):
        return self.jar_name
