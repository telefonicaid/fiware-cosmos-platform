"""
Module bdp_fe.jobconf.models

Create your models here.
"""

from django.db import models

class Job(models.Model):
    """
    A Job is a calculation to be run on the BDP
    """
    date = models.DateTimeField('date created')
