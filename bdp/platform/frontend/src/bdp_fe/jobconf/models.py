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

    def start(self, cluster):
        model = CustomJobModel.objects.get(job=self) # FIXME: non polymorfic
        success = model.start(cluster)
        if success:
            job.status = Job.RUNNING
            job.save()
        return success

    def data_upload(self, upload, cluster):
        """
        Accepts the upload file and moves it to the cluster.

        """
        target_dir = path.join(settings.LANDING_ROOT, self.user.username)
        target_name = "job_%d.data" % self.id
        upload_util.save(upload, target_dir, target_name)
        LOGGER.info("Data saved as %s" % target_name)

        try:
            target_file = path.join(target_dir, target_name)
            LOGGER.debug("Uploading %s to hdfs (%s)" % (target_file,
                                                        self.hdfs_data_path()))
            cluster.copyToHdfs(target_file, self.hdfs_data_path())
            LOGGER.info("Data uploaded to the cluster")
            return True
        except Exception, ex:
            LOGGER.exception(ex)
            return False

    def hdfs_base(self):
        return "/bdp/user/%s/job_%d" % (self.user.username, self.id)

    def hdfs_data_path(self):
        return "%s/data/input/part-r0000.data" % self.hdfs_base()

    def hdfs_output_path(self):
        return "%s/data/output/" % self.hdfs_base()


class JobModel(models.Model):
    """
    Job model represents the calculation to be performed.

    """
    job = models.OneToOneField(Job)

    def start(self, cluster):
        LOGGER.error("Should not invoke JobModel#start")
        return False


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
        upload_util.save(upload, self.landing_dir(), self.jar_name())
        LOGGER.info("Custom JAR saved locally as %s" % self.jar_path())

        # TODO: check the upload is a valid JAR
        return True

    def landing_dir(self):
        return path.join(settings.LANDING_ROOT, self.job.user.username)

    def jar_name(self):
        return "job_%d.jar" % self.job.id

    def jar_path(self):
        return path.join(self.landing_dir(), self.jar_name())

    def start(self, cluster):
        try:
            cluster.runJob(self.jar_path(),
                           self.job.hdfs_data_path(),
                           self.job.hdfs_output_path(),
                           self.mongo_url())
            return True
        except Exception, ex:
            LOGGER.exception(ex)
            return False

    def mongo_url(self):
        return "%s/%s.%s" % (settings.CLUSTER_CONF.get('mongobase'),
                             self.job.user.username,
                             self.job.id)
