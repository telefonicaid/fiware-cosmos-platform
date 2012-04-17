# -*- coding: utf-8 -*-
from desktop.lib.django_util import render

from cosmos.models import JobRun


def index(request):
    """List job runs."""

    return render('index.mako', request, {
        'job_runs': (JobRun.objects.filter(user=request.user)
                                   .order_by('-start_date'))
    })

def list_datasets(request):
    return render('dataset_list.mako', request, dict())

def list_jars(request):
    return render('jar_list.mako', request, dict())
