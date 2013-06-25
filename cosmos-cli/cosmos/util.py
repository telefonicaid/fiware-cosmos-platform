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


class ExitWithError(Exception):
    """To be thrown on a fatal condition. With exit_code you can control the
    command exit code."""

    def __init__(self, exit_code, explanation):
        super(ExitWithError, self).__init__(explanation)
        self.exit_code = exit_code
        self.explanation = explanation


class ResponseError(ExitWithError):
    """Translates failed Responses into an exit code and a explanation."""

    def __init__(self, cause, response):
        try:
            exception = response.json()
            if exception.has_key('error'):
                message = exception['error']
            else:
                message = exception['RemoteException']['message']
        except (ValueError, KeyError):
            message = response.text
        if response.status_code == 401:
            error = "Unauthorized request"
        else:
            error = "Error (%d)" % response.status_code
        super(ResponseError, self).__init__(
            response.status_code,
            ("%s\n%s: %s\n" % (cause, error, message))
        )
