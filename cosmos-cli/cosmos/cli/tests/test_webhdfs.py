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
import os.path
import unittest
from StringIO import StringIO

import requests
from mock import MagicMock, patch
from testfixtures import TempDirectory

import cosmos.cli.webhdfs as webhdfs
from cosmos.cli.util import ExitWithError
from cosmos.common.exceptions import ResponseError
from cosmos.common.tests.util import mock_response


class WebHdfsClientTest(unittest.TestCase):

    def setUp(self):
        self.client = MagicMock()
        self.instance = webhdfs.WebHdfsClient(
            'webhdfs://namenode:1234/', 'user1', client=self.client)
        self.namenode_base = 'http://namenode:1234/webhdfs/v1/user/user1'
        self.datanode_base = 'http://datanode:5678/webhdfs/v1/user/user1'
        self.config = MagicMock()
        self.config.credentials = ("user", "pass")
        self.config.api_url = 'http://host:port/endpoint/v1'

    def test_file_upload(self):
        with TempDirectory() as local_dir:
            redirection = requests.Response()
            redirection.status_code = 307
            datanode_url = (self.datanode_base +
                            '/remote/path?op=CREATE&user.name=user1')
            redirection.headers['Location'] = datanode_url
            created = requests.Response()
            created.status_code = 201
            self.client.put = MagicMock(side_effect=[redirection, created])

            local_file = local_dir.write('file.txt', 'contents')
            self.instance.put_file(local_file, '/remote/path')

            self.assertEqual(self.client.put.call_count, 2)

            self.client.put.assert_any_call(
                self.namenode_base + '/remote/path',
                allow_redirects=False,
                params={'user.name': 'user1', 'op': 'CREATE'}
            )
            last_put = self.client.put.call_args_list[1]
            self.assertEqual(datanode_url, last_put[0][0])

    def test_file_upload_with_replication_error(self):
        with TempDirectory() as local_dir:
            self.client.put = mock_response(status_code=500, json={
                'RemoteException': {
                    'exception': 'ArrayIndexOutOfBoundsException'
                }
            })
            local_file = local_dir.write('file.txt', 'contents')
            self.assertRaisesRegexp(ResponseError, 'Not redirected',
                                    self.instance.put_file, local_file,
                                    '/remote/path')

    def test_list_path(self):
        self.client.get.return_value = mock_response(json={
            "FileStatuses": {
                "FileStatus": [
                    {"pathSuffix": "a.txt"},
                    {"pathSuffix": "b.txt"}
                ]
            }
        })
        statuses = self.instance.list_path('/some/path')
        self.client.get.assert_called_with(
            self.namenode_base + '/some/path',
            params={'user.name': 'user1', 'op': 'LISTSTATUS'})

    def test_list_nonexistent_path(self):
        self.client.get.return_value = mock_response(status_code=404)
        listing = self.instance.list_path('/some/path')
        self.assertEquals(listing, None)

    def test_get_file(self):
        self.client.get.return_value = mock_response(raw=StringIO("hello"))
        out_file = StringIO("")
        size = self.instance.get_file('/remote/file.txt', out_file)
        self.assertEquals(5, size)
        self.assertEquals("hello", out_file.getvalue())
        out_file.close()

    def test_get_nonexisting_file(self):
        self.client.get.return_value = mock_response(status_code=404)
        out_file = StringIO("")
        self.assertRaisesRegexp(
            ExitWithError, 'File /file.txt does not exist',
            self.instance.get_file, '/file.txt', out_file)
        out_file.close()

    def test_delete_path(self):
        self.client.delete.return_value = mock_response(json={
            "boolean": True
        })
        self.assertTrue(self.instance.delete_path('/remote/file.txt'))
        self.client.delete.assert_called_with(
            self.namenode_base + '/remote/file.txt',
            params={'user.name': 'user1', 'op': 'DELETE', 'recursive': 'false'})

    def test_delete_path_with_error(self):
        self.client.delete.return_value = mock_response(status_code=403)
        self.assertRaises(ResponseError, self.instance.delete_path,
                          '/remote/file.txt')

    def test_recursive_delete_path(self):
        self.client.delete.return_value = mock_response(json={
            "boolean": False
        })
        self.assertFalse(self.instance.delete_path('remote/file.txt',
                                                   recursive=True))
        self.client.delete.assert_called_with(
            self.namenode_base + '/remote/file.txt',
            params={'user.name': 'user1', 'op': 'DELETE', 'recursive': 'true'})

    def test_path_type(self):
        self.instance.list_path = MagicMock(side_effect=[
            None,
            [{ "type": "DIRECTORY",
               "pathSuffix": "subdir" }],
            [{ "type": "FILE",
               "pathSuffix": ""}]
        ])
        self.assertEquals(self.instance.path_type('/file'), 'NONE')
        self.assertEquals(self.instance.path_type('/file'), 'DIRECTORY')
        self.assertEquals(self.instance.path_type('/file'), 'FILE')

    def test_get_client_from_config(self):
        response = mock_response(json={
            'location': 'webhdfs://host:8080/',
            'user': 'username'
        })
        with patch('requests.get', MagicMock(return_value=response)):
            result = webhdfs.client_from_config(self.config)
            self.assertEquals(result.webhdfs_uri, 'webhdfs://host:8080/')
            self.assertEquals(result.username, 'username')

    def test_get_client_raises_for_incompatible_api_version(self):
        self.config.api_url = 'http://host:port/endpoint/v25'
        self.assertRaisesRegexp(ExitWithError, 'API version 25 is unsupported',
                                webhdfs.client_from_config,
                                self.config)

    def test_get_client_from_config_when_unavailable(self):
        response = mock_response(status_code=503)
        with patch('requests.get', MagicMock(return_value=response)):
            self.assertRaises(ResponseError, webhdfs.client_from_config,
                              self.config)

