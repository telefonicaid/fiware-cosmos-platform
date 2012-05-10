# -*- coding: utf-8 -*-
"""Cosmos configuration."""

from desktop.lib.conf import Config

MONGO_BASE = Config(key='mongo_base',
                    help='Base URL for mongodb output databases')

def config_validator():
    """
    config_validator() -> [(config_variable, error_msg)] or None
    Called by core check_config() view.
    """
    res = []
    if not MONGO_BASE.get():
        res.append((MONGO_BASE, "This variable must be set"))
    return res
