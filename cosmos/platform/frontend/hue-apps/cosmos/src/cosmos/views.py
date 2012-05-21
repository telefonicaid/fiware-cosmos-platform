# -*- coding: utf-8 -*-
import logging

from desktop.lib.django_util import PopupException, render, render_to_string
from django.core.paginator import Paginator, EmptyPage, PageNotAnInteger
from django.core.urlresolvers import reverse
from django.forms.util import ErrorList
from django.http import Http404
from django.shortcuts import get_object_or_404, redirect
import jobsub.views as jobsub
from jobsub.models import Submission
from jobsubd.ttypes import (BinHadoopStep, LocalizedFile, LocalizeFilesStep,
                            State, SubmissionPlan, SubmissionPlanStep)

from cosmos import conf, mongo, paths
from cosmos.models import JobRun
from cosmos.forms import RunJobForm

LOGGER = logging.getLogger(__name__)
TEMP_JAR = "tmp.jar"
DEFAULT_PAGE = 1

def index(request):
    """List job runs."""

    return render('index.mako', request, dict(
        job_runs=JobRun.objects.filter(user=request.user)
                               .order_by('-start_date')
    ))


def validate_hdfs_path(fs, form, field):
    has_file = fs.exists(form.cleaned_data[field])
    if not has_file:
        errors = form._errors.setdefault(field, ErrorList())
        errors.append("File does not exist")
    return has_file


def submit(job):
    LOGGER.info("Submitting job %s (%d) for %s with JAR=%s on dataset %s" %
                (job.name, job.id, job.user.username, job.jar_path,
                 job.dataset_path))

    lf = LocalizedFile(target_name=TEMP_JAR, path_on_hdfs=job.jar_path)
    lfs = LocalizeFilesStep(localize_files=[lf])
    bhs = BinHadoopStep(arguments=job.hadoop_args(TEMP_JAR))
    plan = SubmissionPlan(name=job.name,
                          user=job.user.username,
                          groups=job.user.get_groups(),
                          steps=[SubmissionPlanStep(localize_files_step=lfs),
                                 SubmissionPlanStep(bin_hadoop_step=bhs)])
    submission = Submission(owner=job.user,
                            name=job.name,
                            last_seen_state=State.SUBMITTED,
                            submission_plan=plan)
    submission.save()
    job.submission = submission
    job.save()
    try:
        try:
            submission.submission_handle = jobsub.get_client().submit(plan)
        except Exception:
            import ipdb; ipdb.set_trace()
            submission.last_seen_state = State.ERROR
            raise
    finally:
        submission.save()


def run_job(request):
    """Starts a new job."""

    if request.method == 'POST':
        form = RunJobForm(request.POST, request.FILES)

        if form.is_valid():
            has_jar = validate_hdfs_path(request.fs, form, "jar_path")
            has_dataset = validate_hdfs_path(request.fs, form, "dataset_path")
            if has_jar and has_dataset:
                data = form.cleaned_data
                job = JobRun(name=data['name'], description=data['description'],
                             user=request.user,
                             dataset_path=data['dataset_path'],
                             jar_path=data['jar_path'])
                job.save()
                submit(job)
                return redirect(reverse('list_jobs'))
    else:
        form = RunJobForm()

    return render('job_run.mako', request, dict(
        form=form
    ))


def ensure_dir(fs, path):
    """Makes sure a directory with a given path exists or it is created.
       Raises a popup is a file is in the way"""
    if fs.isdir(path):
        return
    if fs.exists(path):
        raise PopupException(('A file named "%s" is preventing the creation ' + 
                              'of the upload directory. Please, rename it to ' +
                              'proceed.') % path)
    else:
        fs.mkdir(path)


def upload_index(request):
    """Allows to upload datasets and jars."""
    
    datasets_base = paths.datasets_base(request.user)
    ensure_dir(request.fs, datasets_base)
    jars_base = paths.jars_base(request.user)
    ensure_dir(request.fs, jars_base)
    return render('upload_index.mako', request, dict(
        datasets_base=datasets_base,
        jars_base=jars_base
    ))


def show_results(request, job_id):
    job = get_object_or_404(JobRun, pk=job_id, user=request.user)
    if job.submission is None:
        raise Http404

    try:
        primary_key = request.GET.get('primary_key')
        paginator = mongo.retrieve_results(job.id, primary_key)

        try:
            page_num = request.GET.get('page', DEFAULT_PAGE)
        except ValueError:
            page_num = DEFAULT_PAGE

        try:
            page = paginator.page(page_num)
        except PageNotAnInteger:
            page = paginator.page(DEFAULT_PAGE)
        except EmptyPage:
            page = paginator.page(paginator.num_pages)

        prototype_result = page.object_list[0]
        if primary_key is None:
            primary_key = prototype_result.pk

        return render('job_results.mako', request, dict(
            title='Results of job %s' % job.id,
            page=page,
            hidden_keys=mongo.HIDDEN_KEYS,
            primary_key=primary_key
        ))

    except mongo.NoResultsError:
        return render('job_results.mako', request, dict(
            title='Results of job %s' % job.id,
            hidden_keys=mongo.HIDDEN_KEYS
        ))

    except mongo.NoConnectionError:
        raise PopupException('Database not available')
