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


class OperationError(CosmosException):
    """Represents a unexpected condition happent when performing an API
    operation.

    It just keeps the message.
    >>> ex = OperationError('Out of foo while doing bar')
    >>> ex.message
    'Out of foo while doing bar'
    """

    def __init__(self, message):
        self.message = message
        super(OperationError, self).__init__(message)
