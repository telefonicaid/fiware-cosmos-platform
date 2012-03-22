"""
Custom job model.

Consists on a user-provided jar to be deployed on the cluster.
"""

import logging
from os import makedirs, path

from django.conf import settings

from models import Job, JobModel

LOGGER = logging.getLogger(__name__)

def handle_upload(job, upload):
    """
    Accepts the upload file and moves it to the cluster.

    """
    ensure_dir(settings.LANDING_ROOT)
    target_file = path.join(settings.LANDING_ROOT, "job_%d.jar" % job.id)
    with open(target_file, 'w') as dest:
        for chunk in upload.chunks():
            dest.write(chunk)
    LOGGER.info("Upload saved to %s" % target_file)

    # TODO: check the upload is a valid JAR
    # TODO: upload to the server
    return True

def ensure_dir(dir_path):
    """Creates the dir when not exists"""
    if not path.exists(dir_path):
        makedirs(dir_path)
