"""
Job status updater

"""
import logging

from django.conf import settings
from django.core.management.base import BaseCommand, CommandError

from bdp_fe.jobconf.models import Job
from bdp_fe.jobconf.cluster import remote

LOGGER = logging.getLogger(__name__)
CLUSTER = remote.Cluster(settings.CLUSTER_CONF.get('host'),
                         settings.CLUSTER_CONF.get('port'))

class Command(BaseCommand):
    args = ''
    help = 'Updates all job status by polling the backend'

    def handle(self, *args, **options):
        updated = 0
        try:
            for job in Job.objects.all():
                if job.status == Job.RUNNING:
                    job.status = CLUSTER.getJobStatus(job.execution_id)
                    job.save()
                    updated += 1
            self.stdout.write("%d updates\n" % updated)
        except remote.ConnException:
            self.stdout.write("Connection problem")
