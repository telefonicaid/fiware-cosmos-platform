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

