"""
Job status updater

"""
import logging
from optparse import make_option

from django.conf import settings
from django.core.management.base import BaseCommand, CommandError

from bdp_fe.jobconf.models import Job
from bdp_fe.jobconf.cluster import remote

LOGGER = logging.getLogger(__name__)
CLUSTER = remote.Cluster(settings.CLUSTER_CONF.get('host'),
                         settings.CLUSTER_CONF.get('port'))

class Command(BaseCommand):
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
                        result = CLUSTER.getJobResult(job.execution_id)
                        job.status = result.status - 1
                        if result.reason is not None:
                            job.error_code = result.reason.errorCode
                            job.error_message = result.reason.errorMsg
                        job.save()
                        updated += 1
                    except remote.ClusterException, ex:
                        self.stdout.write("Error %d: %s\n" % (ex.error_code,
                                                              ex.error_message))
            if not quiet:
                self.stdout.write("%d updates\n" % updated)
        except remote.ConnException:
            self.stdout.write("Connection problem\n")
