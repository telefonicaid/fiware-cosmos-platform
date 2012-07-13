# -*- coding: utf-8 -*-
"""
Default paths.
"""

def datasets_base(user):
    return '/user/%s/datasets/' % user.username


def jars_base(user):
    return '/user/%s/jars/' % user.username


def tmp_path(user, job_id):
    return '/user/%s/tmp/job_%d/' % (user.username, job_id)
