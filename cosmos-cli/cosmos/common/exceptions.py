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


class ResponseError(CosmosException):
    """Represents failed HTTP responses as a Cosmos exception."""

    def __init__(self, message, response):
        self.message = message
        self.response = response
        super(ResponseError, self).__init__(self.__format_exception())

    def __format_exception(self):
        return "%s: %s\n%s" % (self.__error_heading(),
                               self.message,
                               self.__extract_error_from_body())

    def __error_heading(self):
        if self.response.status_code == 401:
            return "Unauthorized request"
        else:
            return "HTTP error (%d)" % self.response.status_code

    def __extract_error_from_body(self):
        """Extract error information from body.

        Tries to interpret content as simple JSON errors or as WebHDFS errors
        with fallback to the plain text value.
        """
        try:
            exception = self.response.json()
            if exception.has_key('error'):
                return exception['error']
            else:
                return exception['RemoteException']['message']
        except (ValueError, KeyError):
            return self.response.text
