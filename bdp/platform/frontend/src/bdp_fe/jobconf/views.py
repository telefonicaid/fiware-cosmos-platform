"""
Module bdp_fe.jobconf.views

"""

import logging

from django import forms
from django.contrib import auth, messages
from django.contrib.auth.decorators import login_required
from django.core.paginator import Paginator, EmptyPage, PageNotAnInteger
from django.core.urlresolvers import reverse
from django.http import HttpResponseNotFound
from django.shortcuts import get_object_or_404, redirect, render_to_response
from django.template import RequestContext

from bdp_fe.jobconf import custom_model
from bdp_fe.jobconf import data
from bdp_fe.jobconf.models import Job, JobModel
from bdp_fe.jobconf.views_util import safe_int_param, retrieve_results

LOGGER = logging.getLogger(__name__)

@login_required
def list_jobs(request):
    job_id = safe_int_param(request.GET, 'run_job')
    if job_id:
        run_job(request, job_id)

    return render_to_response('job_listing.html', {
        'title': 'Job listing',
        'jobs': Job.objects.all(),
    }, context_instance=RequestContext(request))


@login_required
def view_results(request, job_id):
    job_id = int(job_id)
    job = Job.objects.get(id=job_id)
    primary_key = job.results_primary_key
    results = retrieve_results(job_id, 'word')
    prototype_result = results[0]
    paginator = Paginator(results, 100)
    page = request.GET.get('page')
    if not page:
        page = 1
    try:
        paginated_results = paginator.page(page)
    except PageNotAnInteger:
        paginated_results = paginator.page(1)
    except EmptyPage:
        paginated_results = paginator.page(paginator.num_pages)

    return render_to_response('job_results.html',
                              {'title' : 'Results of job %s' % job_id,
                               'job_results' : paginated_results,
                               'prototype_result': prototype_result,
                               'hidden_keys': ['_id', 'job_id'],
                               'expand_types': ['dict', 'list'],
                               'primary_key': primary_key},
                              context_instance=RequestContext(request))

def run_job(request, job_id):
    try:
        job = Job.objects.get(id=job_id)
        if job.status != Job.CREATED:
            msg = "Cannot start job in %s status" % job.get_status_display()
            messages.warning(request, msg)
            LOGGER.warning(msg)
            return

        job.status = Job.RUNNING
        job.save()

        # TODO: Unimplemented behaviour
        LOGGER.warning("Unimplemented job start")
        messages.info(request, "Job %s was started." % job_id)
    except Job.DoesNotExist:
        messages.warning(request, "Cannot start job %d: not found" % job_id)
        LOGGER.warning("Job %d not found" % job_id)

class NewJobForm(forms.Form):
    name = forms.CharField(max_length=Job.NAME_MAX_LENGTH)
    results_primary_key = forms.CharField(max_length=Job.RESULTS_PK_MAX_LENGTH)

@login_required
def new_job(request):
    if request.method == 'POST':
        form = NewJobForm(request.POST)
        if form.is_valid():
            job = Job(name=form.cleaned_data['name'],
                      user=request.user,
                      results_primary_key=request.user,
                      status=Job.CREATED)
            job.save()
            return redirect(reverse('config_job', args=[job.id]))
    else:
        form = NewJobForm()

    return render_to_response('new_job.html', {
        'title': 'New job',
        'form': form,
    }, context_instance=RequestContext(request))

class UploadJarForm(forms.Form):
    file = forms.FileField()

@login_required
def config_job(request, job_id):
    job = get_object_or_404(Job, pk=job_id, user=request.user)
    if request.method == 'POST':
        form = UploadJarForm(request.POST, request.FILES)
        if form.is_valid() and custom_model.handle_upload(job,
                request.FILES['file']):
            return redirect(reverse('upload_data', args=[job.id]))
        else:
            messages.info(request, 'JAR file upload failed')
    else:
        form = UploadJarForm()
    return render_to_response('upload_jar.html', {
        'title': 'Configure custom job',
        'job': job,
        'form': form,
    }, context_instance=RequestContext(request))

class UploadDataForm(forms.Form):
    file = forms.FileField()

@login_required
def upload_data(request, job_id):
    job = get_object_or_404(Job, pk=job_id, user=request.user)
    if request.method == 'POST':
        form = UploadDataForm(request.POST, request.FILES)
        if form.is_valid() and data.handle_upload(job, request.FILES['file']):
            return redirect(reverse('list_jobs'))
        else:
            messages.info(request, 'Data file upload failed')
    else:
        form = UploadDataForm()
    return render_to_response('upload_data.html', {
        'title': 'Upload job %s data' % job.name,
        'job': job,
        'form': form,
    }, context_instance=RequestContext(request))
