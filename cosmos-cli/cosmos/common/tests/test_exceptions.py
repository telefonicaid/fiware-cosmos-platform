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
import unittest

from mock import MagicMock

from cosmos.common.exceptions import ResponseError
from cosmos.common.tests.util import mock_response


class ResponseErrorTest(unittest.TestCase):

    def test_keep_attributes(self):
        message = "trying to foo"
        response = mock_response()
        ex = ResponseError(message, response)
        self.assertEquals(message, ex.message)
        self.assertEquals(response, ex.response)

    def test_format_authorization_errors(self):
        ex = ResponseError("message", mock_response(status_code=401))
        self.assertIn("Unauthorized request", str(ex))

    def test_format_non_authorization_errors(self):
        ex = ResponseError("message", mock_response(status_code=415))
        self.assertIn("HTTP error (415)", str(ex))

    def test_format_simple_error_messages(self):
        ex = ResponseError("message", mock_response(json={
            'error': 'simple error'
        }))
        self.assertIn('simple error', str(ex))

    def test_format_webhdfs_error_messages(self):
        ex = ResponseError("message", mock_response(json={
            'RemoteException': {
                'message': 'long Java stacktrace'
            },
            'IrrelevantStuff': 'skipped'
        }))
        self.assertIn('long Java stacktrace', str(ex))
        self.assertNotIn('skipped', str(ex))

    def test_format_plain_text_error_messages(self):
        ex = ResponseError('message', mock_response(text='plain error'))
        self.assertIn('plain error', str(ex))

