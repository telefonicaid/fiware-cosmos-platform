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
import os
import re

from cosmos.cli.terminal import get_terminal_size


UUID_PATTERN = re.compile(r"^[0-9a-f]{32}$")
DEFAULT_CONSOLE_WIDTH = 80


def last_cluster_filename():
    """Get the filename in which last cluster id is saved, regardless of the
    operative system.

    >>> "cosmoslast" in last_cluster_filename()
    True
    """
    if os.name == "nt" and os.getenv("USERPROFILE"):
        return os.path.join(os.getenv("USERPROFILE").decode("mbcs"),
                            "Application Data", "cosmoslast.txt")
    else:
        return os.path.expanduser("~/.cosmoslast")


def get_last_cluster_id():
    """Retrieves the last cluster id or raises an argparse exception."""
    try:
        filename = last_cluster_filename()
        with open(filename) as f:
            last_cluster = f.read().strip()
        if not UUID_PATTERN.match(last_cluster):
            raise argparse.ArgumentTypeError((
                "Invalid last cluster saved at %s: '%s'.\n"
                "Please edit or remove the file."
            ) % (filename, last_cluster))
        else:
            print "Executing command on last cluster (%s)" % last_cluster
            return last_cluster
    except IOError:
        raise argparse.ArgumentTypeError("No 'last' cluster")


def set_last_cluster_id(last_cluster):
    """Resets the last cluster id file. A warning message is printed when the
    file cannot be written."""
    filename = last_cluster_filename()
    try:
        with open(filename, mode="w") as f:
            f.write(last_cluster)
    except IOError:
        print "Warning: cannot save last cluster id at " + filename


def valid_cluster_id(argument):
    """Validates that the argument is a well formed UUID or 'last' to reuse
    the last cluster id.

    >>> valid_cluster_id("00000000000000000000000000000000")
    '00000000000000000000000000000000'

    >>> valid_cluster_id("my_cluster")
    Traceback (most recent call last):
        ...
    ArgumentTypeError: Not a valid cluster id

    When using 'last' the .cosmoslast file should contain a valid cluster id.
    """
    if argument == 'last':
        return get_last_cluster_id()
    if not UUID_PATTERN.match(argument):
        raise argparse.ArgumentTypeError("Not a valid cluster id")
    return argument


def add_cluster_id_argument(parser):
    parser.add_argument("cluster_id", type=valid_cluster_id, help="cluster id")


def ellipsize(text, max_width):
    """Truncate long texts with ellipsis.
    >>> ellipsize("Very long text", 10)
    'Very lo...'
    >>> ellipsize("Short", 10)
    'Short'
    >>> ellipsize("Too small width", 2)
    '...'
    """
    ELLIPSIS = '...'

    if len(text) <= max_width:
        return text
    if len(ELLIPSIS) >= max_width:
        return ELLIPSIS
    return text[: max_width - len(ELLIPSIS)] + ELLIPSIS


def at_least_2(argument):
    """Parses integers equal or greater than 2.
    >>> at_least_2('2')
    2
    >>> at_least_2('1')
    Traceback (most recent call last):
        ...
    ArgumentTypeError: 1 is less than 2
    """
    amount = int(argument)
    if amount <= 1:
        raise argparse.ArgumentTypeError("%r is less than 2" % amount)
    return amount


def filtered(json, filter_keys):
    """ Returns a dict of key-value pairs filtering out the specified keys.

    >>> filtered({'c': 'C', 'xox': '!!', 'a': 'A'}, ['xox'])
    {'a': 'A', 'c': 'C'}
    """
    return {k: v for k, v in json.iteritems() if k not in filter_keys}


def get_console_width():
    try:
        return get_terminal_size()[0]
    except:
        return DEFAULT_CONSOLE_WIDTH
