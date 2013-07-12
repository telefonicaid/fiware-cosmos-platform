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

import requests
from mock import MagicMock, patch

from cosmos.common.exceptions import (ResponseError,
                                      UnsupportedApiVersionException)
from cosmos.common.tests.util import mock_response
from cosmos.storage.connection import connect


API_KEY='AL2jHQ25a1I3Bb4ZCUzs'
API_SECRET='40EelasvfAtiTiFwHvsiaQLNrut4wfGS2E4TTA3m'
API_URL='http://host:port/endpoint/v1'


class ConnectionTest(unittest.TestCase):

    def test_connect(self):
        response = mock_response(json={
            'location': 'webhdfs://host:8080/',
            'user': 'username'
        })
        with patch('requests.get', MagicMock(return_value=response)):
            result = connect(API_KEY, API_SECRET, API_URL)
            client = result._StorageConnection__client
            self.assertEquals(client.webhdfs_uri, 'webhdfs://host:8080/')
            self.assertEquals(client.username, 'username')

    def test_connect_with_incompatible_api_version(self):
        incompatible_api = 'http://host:port/endpoint/v25'
        self.assertRaisesRegexp(UnsupportedApiVersionException,
                                'Unsupported API version 25',
                                connect, API_KEY, API_SECRET, incompatible_api)

    def test_connect_when_service_is_unavailable(self):
        response = mock_response(status_code=503)
        with patch('requests.get', MagicMock(return_value=response)):
            self.assertRaises(ResponseError, connect, API_KEY, API_SECRET,
                              API_URL)
