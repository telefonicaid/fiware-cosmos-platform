"""
Data models.

"""
import logging
from os import path

from django.conf import settings
from django.contrib.auth.models import User
from django.db import models

from cosmos.jobconf import upload_util
from cosmos.jobconf.cluster import remote


LOGGER = logging.getLogger(__name__)


def trim_to(self, text, max_length):
    """Limits to max_length characters

    An ellipsis is added at the end of the trimmed text
    """

    if len(text) > max_length:
        return text[:max_length - 4] + "\n..."
    else:
        return text


class Job(models.Model):
    """
    A Job is a calculation to be run on the BDP
    """

    NAME_MAX_LENGTH = 40
    name = models.CharField(max_length = NAME_MAX_LENGTH)

    user = models.ForeignKey(User)

    date = models.DateTimeField(auto_now = True)

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
    status = models.IntegerField(choices = JOBSTATUS_CHOICES)

    INPUT_DATA_MAX_LENGTH = 256
    input_data = models.CharField(null = True, blank = True,
                                  max_length = INPUT_DATA_MAX_LENGTH)

    UNKNOWN = 0
    FILE_COPY_FAILED = 1
    RUN_JOB_FAILED = 2
    INVALID_JOB_ID = 3
    CLUSTER_ERROR_CHOICES = (
        (UNKNOWN, 'Unknown error'),
        (FILE_COPY_FAILED, 'File transfer failed'),
        (RUN_JOB_FAILED, 'Job execution failed'),
        (INVALID_JOB_ID, 'Invalid job ID'),
    )
    error_code = models.IntegerField(choices = CLUSTER_ERROR_CHOICES,
                                     null = True, blank = True)

    ERROR_MESSAGE_MAX_LENGTH = 4096
    error_message = models.CharField(null = True, blank = True,
                                     max_length = ERROR_MESSAGE_MAX_LENGTH)

    def start(self, cluster):
        """Returns true on success."""
        model = CustomJobModel.objects.get(job = self) # FIXME: non polymorfic
        try:
            model.start(cluster)
            LOGGER.info("Job %d started" % self.id)
            self.status = Job.RUNNING
            self.save()
            return True

        except remote.ClusterException, ex:
            LOGGER.info("Cannot start job %d: error %d" % (self.id,
                                                           ex.error_code))
            self.set_error(ex)

        except Exception, ex:
            LOGGER.info("Cannot start job %d: %s" % (self.id, ex))
            self.error_code = Job.UNKNOWN
            self.error_message = trim_to(str(ex), Job.ERROR_MESSAGE_MAX_LENGTH)
        self.status = Job.FAILED
        self.save()
        return False


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
            LOGGER.error("Cannot upload data file: %s" % ex)
            return False

    def is_runnable(self):
        return (self.status == Job.CONFIGURED and self.input_data is not None
                and len(self.input_data) > 0)

    def set_error(self, exception):
        """Updates error info from any exception with error code and message"""
        try:
            self.error_code = exception.errorCode
            self.error_message = exception.errorMsg
        except AttributeError:
            self.error_code = exception.error_code
            self.error_message = exception.error_message
        self.error_message = trim_to(self.error_message,
                                     Job.ERROR_MESSAGE_MAX_LENGTH)

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
    jar_name = models.CharField(max_length = 256, null = True, blank = True)

    def jar_upload(self, upload):
        """
        Accepts the upload file and moves it to the cluster.
        Returns true on success.

        """
        upload_util.save(upload, self.landing_dir(), self.jar_basename())
        LOGGER.info("Custom JAR saved locally as %s" % self.jar_path())

        # TODO: check the upload is a valid JAR
        return True

    def landing_dir(self):
        return path.join(settings.LANDING_ROOT, self.job.user.username)

    def jar_basename(self):
        return "job_%d.jar" % self.job.id

    def jar_path(self):
        return path.join(self.landing_dir(), self.jar_basename())

    def start(self, cluster):
        cluster.runJob(str(self.job.id),
                       self.jar_path(),
                       self.job.hdfs_data_path(),
                       self.job.hdfs_output_path(),
                       self.mongo_url())

    def mongo_url(self):
        return "%s/%s.%s" % (self.mongo_base(),
                             self.mongo_db(),
                             self.mongo_collection())

    def mongo_base(self):
        return "%s" % settings.CLUSTER_CONF.get('mongobase')

    def mongo_db(self):
        return "%s"% self.job.user.username

    def mongo_collection(self):
        return "job_%s" % self.job.id

    def __unicode__(self):
        return "Custom model %s" % self.jar_name
