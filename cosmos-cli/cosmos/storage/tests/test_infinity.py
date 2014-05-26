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
from StringIO import StringIO

import json
import os
import requests
from mock import MagicMock, patch
from testfixtures import TempDirectory

import cosmos.storage.infinity as webhdfs
from cosmos.storage.infinity import DirectoryListing, FileMetadata
from cosmos.common.exceptions import OperationError
from cosmos.common.exceptions import ResponseError
from cosmos.common.tests.util import mock_response


class DirectoryListingTest(unittest.TestCase):
    def setUp(self):
        self.empty_dir_listing = DirectoryListing()
        file1 = {
            "path" : "/usr/gandalf/spells.txt",
            "type" : "file",
            "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/spells.txt",
            "owner" : "gandalf",
            "group" : "istari",
            "permissions" : "600",
            "size" : 45566918656,
            "modificationTime" : "2014-04-08T12:41:34+0100",
            "accessTime" : "2014-04-08T12:54:32+0100",
            "blockSize" : 65536,
            "replication" : 3
        }
        file2 = {
            "path" : "/usr/gandalf/enemies",
            "type" : "directory",
            "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies",
            "owner" : "gandalf",
            "group" : "istari",
            "permissions" : "750",
            "size" : 0,
            "modificationTime" : "2014-04-08T12:55:45+0100",
            "accessTime" : "2014-04-08T13:01:22+0100",
            "blockSize" : 0,
            "replication" : 0
        }
        self.one_file_listing = DirectoryListing(statuses=[file1])
        self.two_files_listing = DirectoryListing(statuses=[file1, file2])

    def test_listing_is_iterable(self):
        self.assert_iterable_length(self.empty_dir_listing, 0)
        self.assert_iterable_length(self.two_files_listing, 2)

    def test_listing_has_readable_str(self):
        self.assertEquals(str(self.empty_dir_listing),
                          'Listing(0 files)')
        self.assertEquals(str(self.one_file_listing),
                          'Listing(1 file)')
        self.assertEquals(str(self.two_files_listing),
                          'Listing(2 files)')

    def test_path_type(self):
        self.assertEquals(DirectoryListing(statuses=None).path_type, 'directory')
        self.assertEquals(DirectoryListing(
            statuses=[{"type": "FILE", "pathSuffix": ""}]).path_type, 'directory')
        self.assertEquals(self.two_files_listing.path_type, 'directory')
        self.assertEquals(FileMetadata(the_json=None).path_type, 'file')

    def assert_iterable_length(self, iterable, expected_length):
        self.assertEquals(sum(1 for _ in iterable), expected_length)


class InfinityClientTest(unittest.TestCase):
    def setUp(self):
        self.client = MagicMock()
        key = 'key'
        secret = 'secret'
        self.instance = webhdfs.InfinityClient(
            'http://namenode:1234/', 'user1', key, secret, client=self.client)
        self.namenode_base = 'http://namenode:1234/infinityfs/v1/metadata/user/user1'
        self.datanode_base = 'http://datanode:5678/infnityfs/v1/content/user/user1'
        self.config = MagicMock()
        self.config.credentials = ("user", "pass")
        self.config.api_url = 'http://host:port/endpoint/v1'
        self.auth = (key, secret)

    def test_file_upload(self):
        with TempDirectory() as local_dir:
            file_created_json = {
                "path": "/user/user1/remote/path",
                "metadata": "%s/remote/path" % self.namenode_base,
                "content": "%s/remote/path" % self.datanode_base,
                "type": "file",
                "owner": "user1",
                "group": "users",
                "permissions": "640",
                "size": 0,
                "modificationTime": "2014-04-08T12:31:45+0100",
                "accessTime": "2014-04-08T12:45:22+0100",
                "blockSize": 67108864,
                "replication": 3
            }
            file_creation = mock_response(status_code=201, json=file_created_json)
            data_saved = requests.Response()
            data_saved.status_code = 204
            self.client.post = MagicMock(side_effect=[file_creation])
            self.client.put = MagicMock(side_effect=[data_saved])

            local_file = local_dir.write('file.txt', 'contents')
            self.instance.put_file(local_file, '/remote/path')

            self.assertEqual(self.client.post.call_count, 1)
            self.assertEqual(self.client.put.call_count, 1)

            self.client.post.assert_any_call(
                self.namenode_base + '/remote',
                auth=self.auth,
                data=json.dumps({
                    "action": "mkfile",
                    "name": "path",
                    "permissions": "640"
                })
            )
            self.client.put.assert_any_call(
                self.datanode_base + '/remote/path',
                auth=self.auth,
                data=local_file
            )

    def test_list_path(self):
        statuses = [
            {"path": "a.txt"},
            {"path": "b.txt"}
        ]
        self.client.get.return_value = mock_response(json={
            "type": "directory",
            "content": statuses
        })
        self.assertEquals(self.instance.list_path('/some/path').statuses,
                          statuses)
        self.client.get.assert_called_with(
            self.namenode_base + '/some/path',
            auth=self.auth)

    def test_list_nonexistent_path(self):
        self.client.get.return_value = mock_response(status_code=404)
        listing = self.instance.list_path('/some/path')
        self.assertIsNone(listing)

    def test_invalid_permissions_chmod(self):
        self.assertRaises(OperationError, self.instance.chmod, '/some/path', '1111')
        self.assertRaises(OperationError, self.instance.chmod, '/some/path', '118')
        self.assertRaises(OperationError, self.instance.chmod, '/some/path', '4')

    def test_chmod(self):
        self.client.post.return_value = mock_response(status_code=204)
        self.instance.chmod('/some/path', '777')
        self.client.post.assert_called_with(
            self.namenode_base + '/some/path',
            auth=self.auth,
            data=json.dumps({'action': 'chmod', 'permissions': '777'})
        )

    def test_get_file(self):
        self.client.get.side_effect = self._file_list_and_content("file.txt", "hello")
        out_file = StringIO("")
        size = self.instance.get_file('/remote/file.txt', out_file)
        self.assertEquals(5, size)
        self.assertEquals("hello", out_file.getvalue())
        out_file.close()

    def test_get_nonexisting_file(self):
        self.client.get.return_value = mock_response(status_code=404)
        out_file = StringIO("")
        self.assertRaisesRegexp(
            OperationError, 'File /file.txt does not exist',
            self.instance.get_file, '/file.txt', out_file)
        out_file.close()

    def test_delete_path(self):
        self.client.delete.return_value = mock_response(status_code=204)
        self.instance.delete_path('/remote/file.txt')
        self.client.delete.assert_called_with(
            self.namenode_base + '/remote/file.txt',
            auth=self.auth,
            params={'recursive': 'false'})

    def test_delete_path_with_error(self):
        self.client.delete.return_value = mock_response(status_code=403)
        self.assertRaises(ResponseError, self.instance.delete_path,
                          '/remote/file.txt')

    def test_recursive_delete_path(self):
        self.client.delete.return_value = mock_response(status_code=204)
        self.assertFalse(self.instance.delete_path('remote/file.txt',
                                                   recursive=True))
        self.client.delete.assert_called_with(
            self.namenode_base + '/remote/file.txt',
            params={'recursive': 'true'},
            auth=self.auth)

    # Utilities
    def _file_list_and_content(self, file_name, content):
        max_chunk_size = 2
        def side_effect(*args, **kwargs):
            if "metadata" in args[0]:
                return mock_response(json={
                    "type": "file",
                    "content": "%s/%s" % (self.datanode_base, os.path.basename(file_name))
                })
            if "content" in args[0]:
                content_length, chunk_size = len(content), len(content)/max_chunk_size
                chunks = [content[i:i+chunk_size] for i in range(0, content_length, chunk_size)]
                return mock_response(chunks=chunks)
        return side_effect


