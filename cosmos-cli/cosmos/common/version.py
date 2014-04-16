# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
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
