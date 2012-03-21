"""
Module bdp_fe.jobconf.views

"""

import logging

from django.contrib import auth
from django.contrib.auth.decorators import login_required
from django.http import HttpResponseNotFound
from django.shortcuts import redirect, render_to_response

logger = logging.getLogger(__name__)

@login_required
def list_jobs(request):
    # TODO: get jobs from db
    return render_to_response('job_listing.html')

@login_required
def new_job(request):
    return HttpResponseNotFound()
