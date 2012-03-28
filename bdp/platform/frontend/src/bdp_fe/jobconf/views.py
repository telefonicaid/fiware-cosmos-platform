"""
Module bdp_fe.jobconf.views

"""
import logging

from django import forms
from django.conf import settings
from django.contrib import auth, messages
from django.contrib.auth.decorators import login_required
from django.core.paginator import Paginator, EmptyPage, PageNotAnInteger
from django.core.urlresolvers import reverse
from django.http import HttpResponse, HttpResponseNotFound, Http404
from django.shortcuts import get_object_or_404, redirect, render_to_response
from django.template import RequestContext, loader

from bdp_fe.jobconf import data
from bdp_fe.jobconf.cluster import remote
from bdp_fe.jobconf.models import CustomJobModel, Job, JobModel
import bdp_fe.jobconf.views_util as util

LOGGER = logging.getLogger(__name__)
CLUSTER = remote.Cluster(settings.CLUSTER_CONF.get('host'),
                         settings.CLUSTER_CONF.get('port'))

@login_required
def list_jobs(request):
    job_id = util.safe_int_param(request.GET, 'run_job')
    reload_period = max(util.safe_int_param(request.GET, 'reload_period',
                                            settings.RELOAD_PERIOD),
                        settings.MIN_RELOAD_PERIOD)
    if job_id:
        run_job(request, job_id)

    return render_to_response('job_listing.html', {
        'title': 'Job listing',
        'jobs': Job.objects.filter(user=request.user,
                                   status__gt=Job.UNCONFIGURED),
        'reload_period': reload_period,
    }, context_instance=RequestContext(request))


@login_required
def view_results(request, job_id):
    job = util.get_owned_job_or_40x(request, job_id)
    if job.status == Job.SUCCESSFUL:
        return view_successful_results(request, job)
    elif job.status == Job.FAILED:
        return view_error(request, job)
    else:
        raise Http404


def view_successful_results(request, job):
    job = util.get_owned_job_or_40x(request, job.id)
    if job.status != Job.SUCCESSFUL:
        raise Http404
    try:
        primary_key = request.GET.get('primary_key')
        results = util.retrieve_results(job.id, primary_key)
        prototype_result = results[0]
        if not primary_key:
            primary_key = prototype_result.pk
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

        context = {'title' : 'Results of job %s' % job.id,
                   'job_results' : paginated_results,
                   'prototype_result': prototype_result,
                   'hidden_keys': util.HIDDEN_KEYS,
                   'expand_types': ['dict', 'list'],
                   'primary_key': primary_key}
        return render_to_response('job_results.html',
                                  context,
                                  context_instance=RequestContext(request))
    except util.NoResultsError:
        context = {'title' : 'Results of job %s' % job.id,
                   'job_results' : None,
                   'hidden_keys': util.HIDDEN_KEYS,
                   'expand_types': ['dict', 'list']}
        return render_to_response('job_results.html',
                                  context,
                                  context_instance=RequestContext(request))
    except util.NoConnectionError:
        context = {'reason': 'Database not available'}
        return HttpResponse(loader.render_to_string("503.html", context),
                            status=503)


def view_error(request, job):
    return render_to_response('error_report.html', {
        'title': 'Error report for %s' % job.name,
        'job': job,
    }, context_instance=RequestContext(request))


def run_job(request, job_id):
    try:
        job = Job.objects.get(id=job_id, user=request.user)
    except Job.DoesNotExist:
        messages.warning(request, "Cannot start job %d: not found" % job_id)
        LOGGER.warning("Job %d not found" % job_id)
        return

    if job.status != Job.CONFIGURED:
        msg = "Cannot start job %s while in %s status" % (
            job.name, job.get_status_display())
        messages.warning(request, msg)
        LOGGER.warning(msg)
        return

    if job.input_data is None or len(job.input_data) == 0:
        msg = "No data for running job %s" % job.name
        messages.warning(request, msg)
        LOGGER.warning(msg)
        return

    if job.start(CLUSTER):
        messages.info(request, "Job %s was started." % job.name)
    else:
        messages.warning(request, "Cannot start job %s." % job.name)


class NewJobForm(forms.Form):
    name = forms.CharField(max_length=Job.NAME_MAX_LENGTH)


@login_required
def new_job(request):
    if request.method == 'POST':
        form = NewJobForm(request.POST)
        if form.is_valid():
            job = Job(name=form.cleaned_data['name'],
                      user=request.user,
                      status=Job.UNCONFIGURED)
            job.save()
            model = CustomJobModel(job=job) # The only option for the moment
            model.save()
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
    job = util.get_owned_job_or_40x(request, job_id)
    if job.status != Job.UNCONFIGURED:
        raise Http404()
    model = JobModel.objects.get(job=job).customjobmodel
    if request.method == 'POST':
        form = UploadJarForm(request.POST, request.FILES)
        if form.is_valid() and model.jar_upload(request.FILES['file']):
            job.status = Job.CONFIGURED
            job.save()
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
    job = util.get_owned_job_or_40x(request, job_id)
    if job.status != Job.CONFIGURED:
        raise Http404()

    if request.method == 'POST':
        form = UploadDataForm(request.POST, request.FILES)
        if form.is_valid() and job.data_upload(request.FILES['file'],
                                               CLUSTER):
            job.input_data = job.hdfs_data_path()
            job.save()
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
