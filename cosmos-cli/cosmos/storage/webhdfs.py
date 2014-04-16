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
import logging as log
from urlparse import urlparse, urljoin

import requests

from cosmos.common.exceptions import (OperationError, ResponseError,
                                      UnsupportedApiVersionException)
from cosmos.common.routes import Routes


SUPPORTED_VERSIONS = [1]
BUFFER_SIZE = 4096


class DirectoryListing(object):
    """Returned when an existing or non-existing directory is listed"""

    def __init__(self, statuses=[], exists=True):
        self.statuses = statuses
        self.exists = exists

    def __iter__(self):
        return self.statuses.__iter__()

    def __str__(self):
        if not self.exists:
            return 'Listing(unexisting path)'
        size = len(self.statuses)
        return 'Listing(%d file%s)' % (size, '' if size == 1 else 's')

    def path_type(self):
        """Checks the type of the listed path returning either 'DIRECTORY',
        'FILE' or 'NONE'."""
        if not self.exists or self.statuses is None:
            return 'NONE'
        if len(self.statuses) == 1 and self.statuses[0]["pathSuffix"] == '':
            return self.statuses[0]["type"]
        return 'DIRECTORY'


class WebHdfsClient(object):

    def __init__(self, webhdfs_uri, username, api_key, api_secret, client=requests):
        self.webhdfs_uri = webhdfs_uri
        self.username = username
        self.api_key = api_key
        self.api_secret = api_secret
        self.client = client

    def opParams(self, operation, **kwargs):
        return dict(kwargs.items() + [
            ('op', operation),
            ('user.name', self.username),
        ])

    def put_file(self, local_file, remote_path):
        datanode_url = self.__request_upload_location(remote_path)
        response = self.client.put(datanode_url, data=local_file)
        if response.status_code != 201:
            raise ResponseError('Cannot upload file to %s' % datanode_url,
                                response)

    def __request_upload_location(self, remote_path):
        response = self.make_call(self.client.put, remote_path,
                                  allow_redirects=False,
                                  params=self.opParams('CREATE'))
        if response.status_code == 201:
            raise OperationError('WebHDFS uploads are not supported on '
                                 '1-machine clusters: an empty file has been '
                                 'created. See #862.')
        if self.__is_replication_exception(response):
            raise OperationError('Cannot replicate file %s blocks' %
                                 remote_path)
        if response.status_code != 307:
            raise ResponseError('Not redirected by the WebHDFS frontend',
                                response)
        return response.headers['Location']

    def list_path(self, path):
        """Lists a directory or check a file status. Returns an instance
        of DirectoryListing."""
        r = self.make_call(self.client.get, path,
                           params=self.opParams('LISTSTATUS'))
        if r.status_code == 200:
            return DirectoryListing(
                statuses=r.json()["FileStatuses"]["FileStatus"])
        elif r.status_code == 404:
            return DirectoryListing(exists=False)
        else:
            raise ResponseError('Cannot list directory %s' % path, r)

    def get_file(self, remote_path, out_file):
        response = self.make_call(self.client.get, remote_path, stream=True,
                                  params=self.opParams('OPEN'))
        if response.status_code == 404:
            raise ResponseError('File %s does not exist' % remote_path,
                                response)
        if response.status_code != 200:
            raise ResponseError('Cannot download file %s' % remote_path,
                                response)
        buf = response.raw.read(BUFFER_SIZE)
        written = 0
        while (len(buf) > 0):
            written += len(buf)
            out_file.write(buf)
            buf = response.raw.read(BUFFER_SIZE)
        return written

    def delete_path(self, path, recursive=False):
        r = self.make_call(self.client.delete, path, params=self.opParams(
            'DELETE', recursive=str(recursive).lower()))
        if r.status_code != 200:
            raise ResponseError('Cannot delete path %s' % path, r)
        return r.json()["boolean"]

    def make_call(self, method, path, **kwargs):
        """Translates a simple path to the related WebHDFS HTTP URL."""
        if path.startswith("/"):
            rel_path = path[1:]
        else:
            rel_path = path
        processedPath = urljoin(
            'http://' + urlparse(self.webhdfs_uri).netloc,
            '/webhdfs/v1/user/%s/%s' % (self.username, rel_path))
        return method(processedPath,
                      auth=(self.api_key, self.api_secret), **kwargs)

    def __is_replication_exception(self, response):
        if response.status_code != 500:
            return False
        try:
            exception = response.json()
        except ValueError:
            return False
        return (exception.get('RemoteException', {}).get('exception') ==
                'ArrayIndexOutOfBoundsException')
