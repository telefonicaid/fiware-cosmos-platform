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
import cosmos.cli.home_dir as home_dir


UUID_PATTERN = re.compile(r"^[0-9a-f]{32}$")
ELLIPSIS = '...'
DEFAULT_CONSOLE_WIDTH = 80


def get_last_cluster_id():
    """Retrieves the last cluster id or raises an argparse exception."""
    current_home = home_dir.get()
    filename = current_home.abs_path(current_home.get_last_cluster_filename())
    try:
        last_cluster = current_home.read_last_cluster()
    except IOError:
        raise argparse.ArgumentTypeError("Cannot read 'last' cluster from " +
                                         filename)
    if last_cluster is None:
        raise argparse.ArgumentTypeError("No 'last' cluster")
    if not UUID_PATTERN.match(last_cluster):
        raise argparse.ArgumentTypeError((
            "Invalid last cluster saved at %s: '%s'.\n"
            "Please edit or remove the file."
        ) % (filename, last_cluster))
    else:
        print "Executing command on last cluster (%s)" % last_cluster
        return last_cluster


def set_last_cluster_id(last_cluster):
    """Resets the last cluster id file. A warning message is printed when the
    file cannot be written."""
    current_home = home_dir.get()
    try:
        current_home.write_last_cluster(last_cluster)
    except IOError:
        print ("Warning: cannot save last cluster id at %s" %
               current_home.get_last_cluster_filename())


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
    if len(text) <= max_width:
        return text
    if len(ELLIPSIS) >= max_width:
        return ELLIPSIS
    return text[: max_width - len(ELLIPSIS)] + ELLIPSIS


def at_least_1(argument):
    """Parses integers equal or greater than 1.
    >>> at_least_1('1')
    1
    >>> at_least_1('0')
    Traceback (most recent call last):
        ...
    ArgumentTypeError: 0 is less than 1
    """
    amount = int(argument)
    if amount < 1:
        raise argparse.ArgumentTypeError("%r is less than 1" % amount)
    return amount


def string_of(max_len):
    """Returns an argparse validator of string of bounded length.

    >>> validator = string_of(5)
    >>> validator("hello")
    'hello'
    >>> validator("toolong")
    Traceback (most recent call last):
        ...
    ArgumentTypeError: "toolong" has more than 5 characters
    """

    def validator(argument):
        if len(argument) > max_len:
            raise argparse.ArgumentTypeError(
                "\"%s\" has more than %d characters" % (argument, max_len)
            )
        else:
            return argument

    return validator


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
