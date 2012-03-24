"""
Data models.

"""

import logging
from os import path

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

    def hdfs_base(self):
        return "/bdp/user/%s/job_%d" % (self.user.username, self.id)

class JobModel(models.Model):
    """
    Job model represents the calculation to be performed.

    """
    job = models.OneToOneField(Job)

    def start(self, cluster):
        pass

    def hdfs_input_path (self):
        return "%s/data/input/part-r0000.data" % self.job.hdfs_base()

    
class CustomJobModel(JobModel):
    """
    Custom models are based on user-provided jars.

    """
    jar_name = models.CharField(max_length=256, null=True, blank=True)

    def jar_upload(self, upload, cluster):
        """
        Accepts the upload file and moves it to the cluster.
        Returns true on success.

        """
        target_dir = path.join(settings.LANDING_ROOT,
                               self.job.user.username)
        target_name = "job_%d.jar" % self.job.id
        upload_util.save(upload, target_dir, target_name)
        LOGGER.info("Custom JAR saved locally as %s" % target_name)

        # TODO: check the upload is a valid JAR
        cluster.copyToHdfs(path.join(target_dir, target_name),
                           self.hdfs_jar_path())
        LOGGER.info("Custom JAR uploaded to the cluster")
        return True

    def start(self, cluster):
        pass

    def hdfs_jar_path (self):
        return "%s/custom.jar" % self.job.hdfs_base()
