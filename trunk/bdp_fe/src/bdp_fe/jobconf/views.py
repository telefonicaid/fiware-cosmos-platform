"""
Module bdp_fe.jobconf.views

"""

import logging

from pymongo import Connection

from django.contrib import auth, messages
from django.contrib.auth.decorators import login_required
from django.http import HttpResponseNotFound
from django.template import RequestContext
from django.shortcuts import redirect, render_to_response

from models import Job, JobModel

LOGGER = logging.getLogger(__name__)

@login_required
def list_jobs(request):
    try:
        job_id = int(request.GET.get('run_job', ''))
    except ValueError:
        job_id = None
    if job_id:
        run_job(request, job_id)

    return render_to_response('job_listing.html', {
        'title': 'Job listing',
        'jobs': Job.objects.all(),
    }, context_instance=RequestContext(request))

@login_required
def new_job(request):
    return HttpResponseNotFound()

def fake_results(job_id):
    new_results = [{"job_id" : job_id,
                    "word" : "Hello",
                    "count" :  1
                   },
                   {"job_id" : job_id,
                    "word" : "world",
                    "count" :  1
                   }] 
    connection = Connection('localhost', 27017)
    db = connection.test_database
    job_results = db.test_collection
    ignore = job_results.insert(new_results)

def retrieve_results(job_id):
    ans = []
    fake_results(job_id)
    connection = Connection('localhost', 27017)
    db = connection.test_database
    job_results = db.test_collection
    for job_result in job_results.find({"job_id" : job_id}):
        ans.append(job_result)
    return ans

@login_required
def view_job(request, job_id):
    results = retrieve_results(job_id)
    return render_to_response('job_results.html',
                              { 'title' : 'Results of job %s' % job_id,
                                'job_results' : results },
                              context_instance=RequestContext(request))

def run_job(request, job_id):
    try:
        job = Job.objects.get(id=job_id)
        job.status = job.RUNNING
        job.save()

        # TODO: Unimplemented behaviour
        LOGGER.warning("Unimplemented job start")
        messages.info(request, "Job %s was started." % job_id)
    except Job.DoesNotExist:
        messages.warning(request, "Cannot start job %d: not found" % job_id)
        LOGGER.warning("Job %d not found" % job_id)
