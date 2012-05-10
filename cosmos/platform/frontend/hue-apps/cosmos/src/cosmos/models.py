"""
Data models.

"""
import logging

from django.contrib.auth.models import User
from django.db import models
from jobsub.models import Submission

from cosmos import conf


LOGGER = logging.getLogger(__name__)
PATH_MAX_LENGTH = 256
STATE_NAMES = {
    1: 'submitted',
    2: 'running',
    3: 'successful',
    4: 'failed',
    5: 'failed'
}

class JobRun(models.Model):
    """
    JobRun corresponds with an execution of a given model.

    """

    NAME_MAX_LENGTH = 40
    name = models.CharField(max_length=NAME_MAX_LENGTH)

    DESCRIPTION_MAX_LENGTH = 1024
    description = models.TextField(max_length=DESCRIPTION_MAX_LENGTH)
    user = models.ForeignKey(User)
    dataset_path = models.CharField(max_length=PATH_MAX_LENGTH)
    jar_path = models.CharField(max_length=PATH_MAX_LENGTH)
    start_date = models.DateTimeField(auto_now=True)
    submission = models.ForeignKey(Submission, null=True)

    def __unicode__(self):
        return self.name

    def hadoop_args(self, jar_name):
        input_path = self.dataset_path
        output_path = '/user/%s/tmp/job_%d/' % (self.user.username, self.id)
        mongo_url = '%s/db_%d.job_%d' % (conf.MONGO_BASE.get(), self.user.id,
                                         self.id)
        return ['jar', jar_name, input_path, output_path, mongo_url]

    def status(self):
        if self.submission is None:
            return "unsubmitted"
        else:
            return STATE_NAMES[self.submission.last_seen_state]
