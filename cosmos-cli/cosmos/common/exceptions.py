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

class CosmosException(Exception):
    """Root class for the Cosmos exception hierarchy.

    This base class preserves all the positional arguments passed and,
    when represented as string, tags them as Cosmos exception.

    >>> ex = CosmosException(['foo', 12])
    >>> str(ex)
    "CosmosException: ['foo', 12]"
    """

    def __init__(self, *args):
        super(CosmosException, self).__init__(*args)

    def __str__(self):
        return "CosmosException: " + super(CosmosException, self).__str__()


class UnsupportedApiVersionException(CosmosException):
    """Raised on API version incompatibilities.

    It is printed as a human readable message.
    >>> ex = UnsupportedApiVersionException(10, [1, 2])
    >>> str(ex)
    'CosmosException: Unsupported API version 10. Supported versions: 1, 2'

    Relevant information is kept as fields.
    >>> ex.tried_version
    10
    >>> ex.supported_versions
    [1, 2]
    """

    def __init__(self, tried_version, supported_versions):
        self.tried_version = tried_version
        self.supported_versions = supported_versions
        super(UnsupportedApiVersionException, self).__init__(
            'Unsupported API version %s. Supported versions: %s' %
            (tried_version, ', '.join(map(str, supported_versions))))
