"""
Module bdp_fe.jobconf.views

"""

import logging

from django.contrib import auth
from django.contrib.auth.decorators import login_required
from django.http import HttpResponseNotFound
from django.template import RequestContext
from django.shortcuts import redirect, render_to_response

from models import Job, JobModel

logger = logging.getLogger(__name__)

@login_required
def list_jobs(request):
    return render_to_response('job_listing.html', {
        'title': 'Job listing',
        'jobs': Job.objects.all(),
    }, context_instance=RequestContext(request))

@login_required
def new_job(request):
    return HttpResponseNotFound()

@login_required
def results(request, job_id):
    return HttpResponseNotFound()
