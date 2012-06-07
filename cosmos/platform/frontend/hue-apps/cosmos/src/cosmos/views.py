# -*- coding: utf-8 -*-
from contextlib import closing
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
from cosmos.jar import InvalidJarFile, JarFile
from cosmos.hdfs_util import CachedHDFSFile
from cosmos.models import JobRun
from cosmos.forms import (BasicConfigurationForm, DefineJobForm,
                          ParameterizeJobForm)

LOGGER = logging.getLogger(__name__)
TEMP_JAR = "tmp.jar"
DEFAULT_PAGE = 1


def index(request):
    """List job runs."""

    job_runs = JobRun.objects.filter(user=request.user).order_by('-start_date')
    for job_run in job_runs:
        job_run.refresh_state()
    return render('index.mako', request, dict(
        job_runs=job_runs
    ))


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
            submission.last_seen_state = State.ERROR
            raise
    finally:
        submission.save()


def job_wizard(request):
    return request.session.get('job_wizard', {})


def clear_job_wizard(request):
    request.session.pop('job_wizard', '')


def update_job_wizard(request, wizard):
    request.session['job_wizard'] = wizard


def define_job(request):
    """Defines a new job in user session."""

    wizard = job_wizard(request)
    job = wizard.get('job', {})

    if request.method != 'POST':
        form = DefineJobForm(initial=job)

    else:
        form = DefineJobForm(request.POST, request.FILES)
        if form.is_valid(request.fs):
            job.update(form.cleaned_data)
            wizard['job'] = job
            update_job_wizard(request, wizard)

            with closing(CachedHDFSFile(request.fs, job['jar_path'])) \
                    as cached_file:
                try:
                    with closing(JarFile(cached_file.local_path())) as jar:
                        wizard['parameterized'] = jar.is_parameterized()
                        if wizard['parameterized']:
                            wizard['parameters'] = [{
                                'name': template.name,
                                'type': template.type,
                                'default_value': template.default_value
                            } for template in jar.parameters()]
                        else:
                            wizard['parameters'] = None
                        return redirect(reverse('configure_job'))
                except InvalidJarFile as ex:
                    errors = form._errors.setdefault('jar_path', ErrorList())
                    errors.append('Invalid JAR: %s' % ex.message)

    return render('job_define.mako', request, dict(
        form=form,
        wizard_nav={'next': True, 'back': False}
    ))


def configure_job(request):
    """Job configuration step."""

    wizard = job_wizard(request)
    if wizard['parameterized']:
        return configure_parameterized_job(request)
    else:
        return configure_basic_job(request)


def configure_basic_job(request):
    wizard = job_wizard(request)
    if request.method != 'POST':
        form = BasicConfigurationForm(data=wizard['job'])
    elif request.POST.has_key('back'):
        return redirect(reverse('define_job'))
    else:
        form = BasicConfigurationForm(request.POST)
        if form.is_valid(request.fs):
            wizard['job'].update(form.cleaned_data)
            update_job_wizard(request, wizard)
            return redirect(reverse('confirm_job'))

    return render('job_configure.mako', request, dict(
        form=form,
        wizard_nav={'next': True, 'back': True}
    ))


def configure_parameterized_job(request):
    wizard = job_wizard(request)
    parameters = wizard['parameters']

    if request.method != 'POST':
        form = ParameterizeJobForm(parameters,
                                   data=wizard.get('parameter_values', None))
    elif request.POST.has_key('back'):
        return redirect(reverse('define_job'))
    else:
        form = ParameterizeJobForm(parameters, data=request.POST)
        if form.is_valid():
            wizard['parameter_values'] = form.cleaned_data
            update_job_wizard(request, wizard)
            return redirect(reverse('confirm_job'))

    return render('job_configure.mako', request, dict(
        form=form,
        wizard_nav={'next': True, 'back': True}
    ))


def confirm_job(request):
    """Confirm and run."""

    wizard = job_wizard(request)
    job_data = wizard.get('job', {})
    job = JobRun(user=request.user)
    try:
        job.name = job_data['name']
        job.description = job_data['description']
        job.jar_path = job_data['jar_path']
    except KeyError:
        return redirect(reverse('define_job'))

    if wizard['parameterized']:
        job.parameters = []
        for param_template in wizard['parameters']:
            param = param_template.copy()
            param['value'] = wizard['parameter_values'][param['name']]
            job.parameters.append(param)
    else:
        job.dataset_path = job_data['dataset_path']

    if request.method != 'POST':
        return render('job_confirm.mako', request, dict(
            wizard,
            wizard_nav={'next': False, 'back': True, 'finish': 'Run job'}
        ))
    elif request.POST.has_key('back'):
        return redirect(reverse('configure_job'))
    else:
        job.save()
        submit(job)
        clear_job_wizard(request)
        return redirect(reverse('list_jobs'))


def cancel_job(request):
    """Cancels and restarts the new job wizard."""
    clear_job_wizard(request)
    return redirect(reverse('define_job'))


def ensure_dir(fs, path):
    """Makes sure a directory with a given path exists or it is created.
       Raises a popup if a file is in the way"""
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
