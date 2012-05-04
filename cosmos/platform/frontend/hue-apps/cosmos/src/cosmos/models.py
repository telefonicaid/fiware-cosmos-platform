"""
Data models.

"""
import logging

from django.contrib.auth.models import User
from django.db import models


LOGGER = logging.getLogger(__name__)

class Dataset(models.Model):
    """
    A dataset is a related set of files grouped into a HDFS path.

    """

    NAME_MAX_LENGTH = 256
    name = models.CharField(max_length=NAME_MAX_LENGTH)

    user = models.ForeignKey(User)

    DESCRIPTION_MAX_LENGTH = 1024
    description = models.TextField(max_length=DESCRIPTION_MAX_LENGTH)

    PATH_MAX_LENGTH = 256
    path = models.CharField(max_length=PATH_MAX_LENGTH)

    def __unicode__(self):
        return self.name

    def set_default_path(self):
        self.path = "/user/%s/datasets/%s/" % (self.user.username, self.name)


class CustomJar(models.Model):
    """
    Custom JARs uploaded by the user.

    """

    NAME_MAX_LENGTH = 256
    name = models.CharField(max_length=NAME_MAX_LENGTH)

    user = models.ForeignKey(User)

    DESCRIPTION_MAX_LENGTH = 1024
    description = models.TextField(max_length=DESCRIPTION_MAX_LENGTH)

    PATH_MAX_LENGTH = 256
    path = models.CharField(max_length=PATH_MAX_LENGTH)

    def __unicode__(self):
        return self.name

    def set_default_path(self):
        self.path = "/user/%s/jars/%s/" % (self.user.username, self.name)


class JobRun(models.Model):
    """
    JobRun corresponds with an execution of a given model.

    """

    NAME_MAX_LENGTH = 40
    name = models.CharField(max_length=NAME_MAX_LENGTH)

    DESCRIPTION_MAX_LENGTH = 1024
    description = models.TextField(max_length=DESCRIPTION_MAX_LENGTH)

    user = models.ForeignKey(User)
    dataset = models.ForeignKey(Dataset)
    model = models.ForeignKey(CustomJar)

    start_date = models.DateTimeField(auto_now = True)

    RUNNING = 1
    SUCCESSFUL = 2
    FAILED = 3
    STATUS_CHOICES = (
        (RUNNING, 'Running'),
        (SUCCESSFUL, 'Successful'),
        (FAILED, 'Failed'),
    )
    status = models.IntegerField(choices = STATUS_CHOICES)

    def __unicode__(self):
        return self.name
