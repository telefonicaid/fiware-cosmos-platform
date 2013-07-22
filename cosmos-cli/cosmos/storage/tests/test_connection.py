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
from testfixtures import TempDirectory

from cosmos.common.exceptions import (OperationError, ResponseError,
                                      UnsupportedApiVersionException)
from cosmos.common.tests.util import mock_response
from cosmos.storage.connection import connect, StorageConnection


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


class StorageConnectionTest(unittest.TestCase):

    def setUp(self):
        self.client = MagicMock()
        self.instance = StorageConnection(self.client)

    def test_upload_filename(self):
        with TempDirectory() as local_dir:
            local_file = local_dir.write('file.txt', 'contents')
            target_path = self.instance.upload_filename(
                local_file, '/re/mote.txt')
            self.assertEquals(target_path, '/re/mote.txt')
            self.assertEquals(self.client.put_file.call_count, 1)
            args = self.client.put_file.call_args[0]
            self.assertEquals(args[0].readlines(), ['contents'])
            args[0].close()
            self.assertEquals(args[1], '/re/mote.txt')

    def test_upload_file(self):
        self.assertUploadFileToRemotePath('/re/mote.txt')

    def test_upload_file_to_target_with_trailing_dash(self):
        self.assertUploadFileToRemotePath('/re/mote/',
                                          renaming_to='/re/mote/file.txt')

    def test_upload_file_to_directory_target(self):
        self.assertUploadFileToRemotePath('/re/mote',
                                          renaming_to='/re/mote/file.txt',
                                          target_type='DIRECTORY')

    def test_upload_file_to_existing_path(self):
        self.assertUploadFileToRemotePath('/re/mote/file.txt',
                                          target_type='FILE',
                                          raising=OperationError)
    def test_download_to_file(self):
        file = MagicMock()
        self.instance.download_to_file('/remote/file.txt', file)
        self.client.get_file.assert_called_once_with('/remote/file.txt', file)

    def test_download_to_filename(self):
        with TempDirectory() as local_dir:
            local_file = local_dir.getpath('file.txt')
            self.assertDownloadToFilename(local_file)

    def test_download_to_existing_directory(self):
        with TempDirectory() as local_dir:
            local_file = local_dir.makedir('dir')
            self.assertDownloadToFilename(local_file,
                                          renaming_to=local_file + '/file.txt')

    def test_download_to_filename_with_trailing_slash(self):
        with TempDirectory() as local_dir:
            local_file = local_dir.getpath('dir') + '/'
            self.assertRaises(IOError, self.instance.download_to_filename,
                              '/remote/file.txt', local_file)

    def assertUploadFileToRemotePath(self, remote_path, renaming_to=None,
                                     target_type=None, raising=None):
        expected_target = remote_path if renaming_to is None else renaming_to
        self.client.path_type = MagicMock(return_value=target_type)
        with TempDirectory() as local_dir:
            with open(local_dir.write('file.txt', 'contents'), 'rb') as fd:
                if raising is None:
                    target_path = self.instance.upload_file(fd, remote_path)
                    self.assertEquals(target_path, expected_target)
                    self.client.put_file.assert_called_once_with(
                        fd, expected_target)
                else:
                    self.assertRaises(raising, self.instance.upload_file,
                                      fd, remote_path)
        self.client.path_type.assert_called_once_with(remote_path)

    def assertDownloadToFilename(self, local_file, renaming_to=None):
        target_file = local_file if renaming_to is None else renaming_to
        self.instance.download_to_filename('/remote/file.txt', local_file)
        self.assertEquals(self.client.get_file.call_count, 1)
        args = self.client.get_file.call_args[0]
        self.assertEquals('/remote/file.txt', args[0])
        self.assertEquals(target_file, args[1].name)

