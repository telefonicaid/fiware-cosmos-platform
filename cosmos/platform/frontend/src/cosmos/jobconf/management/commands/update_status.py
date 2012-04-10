"""
Job status updater

"""
import logging
from optparse import make_option

from django.conf import settings
from django.core.management.base import BaseCommand

from cosmos.jobconf.models import Job
from cosmos.jobconf.cluster import remote


LOGGER = logging.getLogger(__name__)
CLUSTER = remote.Cluster(settings.CLUSTER_CONF.get('host'),
                         settings.CLUSTER_CONF.get('port'))


class Command(BaseCommand):
    """Check for backend status updates
    """
    args = ''
    option_list = BaseCommand.option_list + (make_option(
        '--quiet',
        action='store_true',
        dest='quiet',
        default=False,
        help="Quiet mode. No output is done unless update process fail."),)
    help = 'Updates all job status by polling the backend'

    def handle(self, *args, **options):
        quiet = options.get('quiet')
        updated = 0
        try:
            for job in Job.objects.all():
                if job.status == Job.RUNNING:
                    try:
                        result = CLUSTER.getJobResult(str(job.id))
                        job.status = result.status - 1
                        if result.reason is not None:
                            job.set_error(result.reason)
                        job.save()
                        updated += 1
                    except remote.ClusterException, ex:
                        if ex.error_code == Job.INVALID_JOB_ID:
                            job.status = Job.FAILED
                            job.set_error(ex)
                            job.save()
                            updated += 1
                        self.stdout.write("Error %d: %s\n" % (ex.error_code,
                                                              ex.error_message))
            if not quiet:
                self.stdout.write("%d updates\n" % updated)
        except remote.ConnException:
            self.stdout.write("Connection problem\n")
