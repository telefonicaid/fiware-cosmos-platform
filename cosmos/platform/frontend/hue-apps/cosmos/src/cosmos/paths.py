# -*- coding: utf-8 -*-
"""
Default paths.
"""

def datasets_base(user):
    return '/user/%s/datasets/' % user.username


def jars_base(user):
    return '/user/%s/jars/' % user.username
