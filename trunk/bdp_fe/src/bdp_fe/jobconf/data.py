"""
Data uploading.

"""

import logging
from os import path

from django.conf import settings

import upload_util
from models import Job, JobModel

LOGGER = logging.getLogger(__name__)

def handle_upload(job, upload):
    """
    Accepts the upload file and moves it to the cluster.

    """
    target_dir = path.join(settings.LANDING_ROOT, job.user.username)
    target_name = "job_%d.data" % job.id
    upload_util.save(upload, target_dir, target_name)
    LOGGER.info("Data saved as %s" % target_name)

    # TODO: upload to the server
    return True
