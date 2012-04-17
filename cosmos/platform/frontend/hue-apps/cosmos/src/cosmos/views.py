# -*- coding: utf-8 -*-
from desktop.lib.django_util import render

from cosmos.models import Dataset, JobRun


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


def list_jars(request):
    return render('jar_list.mako', request, dict())
