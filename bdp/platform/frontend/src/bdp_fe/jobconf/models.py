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

    UNCONFIGURED = -1
    CONFIGURED = 0
    RUNNING = 1
    SUCCESSFUL = 2
    FAILED = 3
    JOBSTATUS_CHOICES = (
        (UNCONFIGURED, 'Unconfigured'),
        (CONFIGURED, 'Configured'),
        (RUNNING, 'Running'),
        (SUCCESSFUL, 'Successful'),
        (FAILED, 'Failed'),
    )
    status = models.IntegerField(choices=JOBSTATUS_CHOICES)

    INPUT_DATA_MAX_LENGTH=256
    input_data = models.CharField(null=True, blank=True,
                                  max_length=INPUT_DATA_MAX_LENGTH)
	
    EXECUTION_ID_MAX_LENGTH=256
    execution_id = models.CharField(null=True, blank=True,
                                    max_length=EXECUTION_ID_MAX_LENGTH)


    def start(self, cluster):
	"""Returns true on success."""
        model = CustomJobModel.objects.get(job=self) # FIXME: non polymorfic
        execution_id = model.start(cluster)
        if execution_id is not None:
            LOGGER.info("Execution id is %s" % execution_id)
            self.execution_id = execution_id
            self.status = Job.RUNNING
        else:
            LOGGER.info("Cannot start job %d" % self.id)
            self.status = Job.FAILED
        self.save()

        return execution_id != None

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

    def __unicode__(self):
        return self.name

class JobModel(models.Model):
    """
    Job model represents the calculation to be performed.

    """
    job = models.OneToOneField(Job)

    def start(self, cluster):
	"""Returns an integer execution id or None"""
        raise NotImplementedError("Should not invoke JobModel#start")


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
            return cluster.runJob(self.jar_path(),
                                  self.job.hdfs_data_path(),
                                  self.job.hdfs_output_path(),
                                  self.mongo_url())
        except Exception, ex:
            LOGGER.exception(ex)
            return None

    def mongo_url(self):
        return "%s/%s.job_%s" % (settings.CLUSTER_CONF.get('mongobase'),
                                 self.job.user.username,
                                 self.job.id)
    
    def __unicode__(self):
        return self.jar_name
