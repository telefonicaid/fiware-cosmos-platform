# -*- coding: utf-8 -*-
import logging

from django.core.urlresolvers import reverse
from django.shortcuts import redirect
from desktop.lib.django_util import PopupException, render

from cosmos.models import CustomJar, Dataset, JobRun
from cosmos.forms import UploadDatasetForm, UploadCustomJarForm

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


def upload_to_new_dir(fs, path, upload, user):
    tmp_file = upload.get_temp_path()
    chown(fs, tmp_file, user.username)
    if fs.exists(path):
        raise PopupException('Directory %s already exists' % path)

    dest_file = fs.join(path, upload.name)
    try:
        fs.mkdir(path)
        fs.rename(tmp_file, dest_file)
        LOGGER.info('Upload %s correctly moved to %s' % (upload.name, path))
    except IOError, ex:
        raise PopupException(
            'Failed to rename uploaded temporary file "%s" to "%s": %s'
            % (tmp_file, dest_file, ex))


def upload_dataset(request):
    if request.method == 'POST':
        form = UploadDatasetForm(request.POST, request.FILES)

        if not form.is_valid():
            LOGGER.error("Error uploading dataset: %s" % (form.errors,))
        else:
            name = form.cleaned_data["name"]
            if Dataset.objects.filter(user=request.user, name=name).exists():
                raise PopupException('Dataset "%s" already exists' % name)
            dataset = Dataset(name=name, user=request.user,
                              description=form.cleaned_data['description'])
            dataset.set_default_path()
            upload = request.FILES['hdfs_file']
            upload_to_new_dir(request.fs, dataset.path, upload, request.user)
            dataset.save()
            upload.remove()
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

def upload_jar(request):
    if request.method == 'POST':
        form = UploadCustomJarForm(request.POST, request.FILES)

        if not form.is_valid():
            LOGGER.error("Error uploading custom jar: %s" % (form.errors,))
        else:
            name = form.cleaned_data["name"]
            if CustomJar.objects.filter(user=request.user, name=name).exists():
                raise PopupException('Custom JAR "%s" already exists' % name)
            jar = CustomJar(name=name, user=request.user,
                            description=form.cleaned_data['description'])
            jar.set_default_path()
            upload = request.FILES['hdfs_file']
            upload_to_new_dir(request.fs, jar.path, upload, request.user)
            jar.save()
            upload.remove()
            return redirect(reverse('list_jars'))

    else:
        form = UploadCustomJarForm()
    return render('jar_upload.mako', request, dict(
        form=form,
        flash_upload=request.GET.get('flash_upload', False)
    ))
