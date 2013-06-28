# -*- coding: utf-8 -*-
#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
#
"""Shared utilities for creating subcommands"""


import argparse
import re


UUID_PATTERN = re.compile(r"^[0-9a-f]{32}$")


def valid_cluster_id(argument):
    """Validates that the argument is a well formed UUID.

    >>> valid_cluster_id("00000000000000000000000000000000")
    '00000000000000000000000000000000'

    >>> valid_cluster_id("my_cluster")
    Traceback (most recent call last):
        ...
    ArgumentTypeError: Not a valid cluster id
    """
    if not UUID_PATTERN.match(argument):
        raise argparse.ArgumentTypeError("Not a valid cluster id")
    return argument


def add_cluster_id_argument(parser):
    parser.add_argument("cluster_id", type=valid_cluster_id, help="cluster id")
