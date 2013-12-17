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
"""Reused test utilities like mocks"""

import StringIO
import sys

import mock


def mock_response(status_code=200, json=None, raw=None, text=None):
    response = mock.MagicMock()
    response.status_code = status_code
    if json is None:
        response.json.side_effect = ValueError("Not JSON")
    elif isinstance(json, mock.Mock):
        response.json = json
    else:
        response.json.return_value = json
    if raw is not None:
        response.raw = raw
    if text is not None:
        response.text = text
    return response


class collect_outputs(object):
    """Environment for collecting standard and error outputs."""

    def __init__(self):
        self.stdout = StringIO.StringIO()
        self.stderr = StringIO.StringIO()

    def __enter__(self):
        sys.stdout = self.stdout
        sys.stderr = self.stderr
        return self

    def __exit__(self, type, value, traceback):
        sys.stdout = sys.__stdout__
        sys.stderr = sys.__stderr__


class mocked_standard_descriptors(collect_outputs):
    """Environment for mocking all standard file descriptors"""

    def __init__(self, canned_input):
        super(mocked_standard_descriptors, self).__init__()
        self.stdin = StringIO.StringIO(canned_input)

    def __enter__(self):
        sys.stdin = self.stdin
        return super(mocked_standard_descriptors, self).__enter__()

    def __exit__(self, *args):
        sys.stdin = sys.__stdin__
        return super(mocked_standard_descriptors, self).__exit__(*args)
