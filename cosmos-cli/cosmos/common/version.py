# -*- coding: utf-8 -*-
#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED 'AS IS' WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
#
import platform

import pkg_resources

from cosmos.common.exceptions import UnsupportedApiVersionException


SUPPORTED_VERSIONS = [1]


def assert_supported_version(api_version):
    """Checks that the api version is supported, throwing
    UnsupportedApiVersionException otherwise.

    >>> assert_supported_version(1)
    >>> assert_supported_version(1000)
    Traceback (most recent call last):
      ...
    UnsupportedApiVersionException: CosmosException: Unsupported API version 1000. Supported versions: 1
    """
    if not api_version in SUPPORTED_VERSIONS:
        raise UnsupportedApiVersionException(api_version, SUPPORTED_VERSIONS)


def print_versions():
    """Print version information.

    >>> print_versions()
    Python version: ...
    Cosmos CLI version: ...
    Supported Cosmos API versions: v1
    """
    print "Python version: %s" % platform.python_version()
    print "Cosmos CLI version: %s" % pkg_resources.require("cosmos")[0].version
    version_tags = ["v%s" % version for version in SUPPORTED_VERSIONS]
    print "Supported Cosmos API versions: %s" % ", ".join(version_tags)
