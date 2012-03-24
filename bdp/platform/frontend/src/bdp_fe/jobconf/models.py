"""
Data models.

"""

import logging
import os.path

from django.conf import settings
from django.contrib.auth.models import User
from django.db import models

from bdp_fe.jobconf import upload_util


LOGGER = logging.getLogger(__name__)


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

class JobModel(models.Model):
    """
    Job model represents the calculation to be performed.

    """
    job = models.OneToOneField(Job)

    def start(self, cluster):
        pass
    
class CustomJobModel(JobModel):
    """
    Custom models are based on user-provided jars.

    """
    jar_name = models.CharField(max_length=256, null=True, blank=True)

    def jar_upload(self, upload):
        """
        Accepts the upload file and moves it to the cluster.
        Returns true on success.

        """
        target_dir = os.path.join(settings.LANDING_ROOT,
                                  self.job.user.username)
        target_name = "job_%d.jar" % self.job.id
        upload_util.save(upload, target_dir, target_name)
        LOGGER.info("Custom JAR saved as %s" % target_name)

        # TODO: check the upload is a valid JAR
        # TODO: upload to the server
        return True

    def start(self, cluster):
        pass
