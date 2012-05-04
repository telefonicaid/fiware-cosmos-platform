# -*- coding: utf-8 -*-
import logging

from django.core.urlresolvers import reverse
from django.shortcuts import redirect
from desktop.lib.django_util import PopupException, render

from cosmos.models import CustomJar, Dataset, JobRun
from cosmos.forms import UploadDatasetForm

LOGGER = logging.getLogger(__name__)

def index(request):
    """List job runs."""

    return render('index.mako', request, dict(
        job_runs=JobRun.objects.filter(user=request.user)
                               .order_by('-start_date')
    ))


def list_datasets(request):
    return render('dataset_list.mako', request, dict(
        datasets=Dataset.objects.filter(user=request.user).order_by('name')
    ))


def chown(fs, filename, username):
    try:
        try:
            fs.setuser(fs.superuser)
            fs.chmod(filename, 0644)
            fs.chown(filename, username, username)
        except IOError, ex:
            msg = 'Failed to chown file ("%s") as superuser %s' % (
                filename, fs.superuser)
            LOGGER.exception(msg)
            raise PopupException(msg, detail=str(ex))
    finally:
        fs.setuser(username)


def dataset_path(user, dataset_name):
    return "/user/%s/datasets/%s/" % (user.username, dataset_name)


def upload_dataset(request):
    if request.method == 'POST':
        form = UploadDatasetForm(request.POST, request.FILES)

        if not form.is_valid():
            LOGGER.error("Error uploading dataset: %s" % (form.errors,))
        else:
            name = form.cleaned_data["name"]
            if Dataset.objects.filter(user=request.user, name=name).exists():
                raise PopupException('Datased "%s" already exists' % name)
            dataset = Dataset(name=name, user=request.user,
                              description=form.cleaned_data['description'],
                              path=dataset_path(request.user, name))

            uploaded_file = request.FILES['hdfs_file']
            tmp_file = uploaded_file.get_temp_path()
            chown(request.fs, tmp_file, request.user.username)
            if request.fs.exists(dataset.path):
                raise PopupException('Dataset "%s" already exists' % dest_dir)

            dest_file = request.fs.join(dataset.path, uploaded_file.name)
            try:
                request.fs.mkdir(dataset.path)
                request.fs.rename(tmp_file, dest_file)
                LOGGER.info('Dataset %s uploaded correctly to %s' % (
                    name, dataset.path))
                dataset.save()
            except IOError, ex:
                raise PopupException(
                    'Failed to rename uploaded temporary file "%s" to "%s": %s'
                    % (tmp_file, dest_file, ex))

            uploaded_file.remove()
            return redirect(reverse('list_datasets'))

    else:
        form = UploadDatasetForm()
    return render('dataset_upload.mako', request, dict(
        form=form,
        flash_upload=request.GET.get('flash_upload', False)
    ))


def list_jars(request):
    return render('jar_list.mako', request, dict(
        jars=CustomJar.objects.filter(user=request.user).order_by('name')
    ))
