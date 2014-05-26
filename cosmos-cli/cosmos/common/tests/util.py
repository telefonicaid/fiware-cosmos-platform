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
"""Reused test utilities like mocks"""

import StringIO
import sys

import mock


def mock_response(status_code=200, json=None, chunks=None, text=None):
    response = mock.MagicMock()
    response.status_code = status_code
    if json is None:
        response.json.side_effect = ValueError("Not JSON")
    elif isinstance(json, mock.Mock):
        response.json = json
    else:
        response.json.return_value = json
    if chunks is not None:
        response.iter_content.return_value = chunks
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
