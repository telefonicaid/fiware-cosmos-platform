# -*- coding: utf-8 -*-
"""Cosmos configuration."""

from desktop.lib.conf import Config


MONGO_BASE = Config(key='mongo_base',
                    help='Base URL for mongodb output databases')


MIN_POLLING_INTERVAL = Config(key='min_polling_interval', default=10, type=int,
                              help='Minimum number of seconds between job ' +
                                   'status poll requests')


RESULTS_PER_PAGE = Config(key='results_per_page', default=100, type=int,
                          help='Number of results shown in one page when' +
                               'listing job results')


def config_validator():
    """
    Called by HUE check_config() view.
    Accepted retrun values: [(config_variable, error_msg)] or None
    """
    res = []
    if not MONGO_BASE.get():
        res.append((MONGO_BASE, "This variable must be set"))
    return res
