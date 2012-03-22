"""
Module bdp_fe.jobconf.views

"""

import logging

from pymongo import Connection

from django.contrib import auth
from django.contrib.auth.decorators import login_required
from django.http import HttpResponseNotFound
from django.template import RequestContext
from django.shortcuts import redirect, render_to_response

logger = logging.getLogger(__name__)

@login_required
def list_jobs(request):
    # TODO: get jobs from db
    return render_to_response('job_listing.html', { 'title': 'Job listing'},
                              context_instance=RequestContext(request))

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
