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
from urlparse import urlparse, urljoin

import json
import re
import requests

from cosmos.common.exceptions import (OperationError, ResponseError)


SUPPORTED_VERSIONS = [1]
BUFFER_SIZE = 4096


class DirectoryListing(object):
    """Returned when an existing or non-existing directory is listed"""

    def __init__(self, statuses=None, exists=True):
        if not statuses: statuses = []
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
        if not self.exists or len(self.statuses) == 0:
            return 'NONE'
        if len(self.statuses) == 1:
            return self.statuses[0]["type"]
        return 'DIRECTORY'


class InfinityClient(object):

    def __init__(self, infinity_uri, username, api_key, api_secret, client=requests):
        self.infinity_uri = infinity_uri
        self.username = username
        self.api_key = api_key
        self.api_secret = api_secret
        self.client = client
        self.metadata = 'metadata'
        self.content = 'content'

    def put_file(self, local_file, remote_path):
        (base_path, filename) = str(remote_path).rsplit('/', 1)
        create_file_body = json.dumps({
            "action": "mkfile",
            "name": filename
        })
        response = self.make_call(self.client.post, base_path, self.metadata,
                                  data=create_file_body)
        if response.status_code != 201:
            # TODO: Add better error messages
            error_messages = {
                404: 'The parent directory does not exist',
                409: 'There is a directory entry with the given file name',
                422: 'The parent path where the new file or directory is to be created is not a directory'
            }
            error_message = error_messages.get(response.status_code,
                                               'Unspecified error')
            raise ResponseError('Cannot create file %s: %s' % (remote_path, error_message),
                                response)
        content_url = response.json()["content"]
        response = self.client.put(content_url, data=local_file,
                                   auth=(self.api_key, self.api_secret))
        if response.status_code != 204:
            raise ResponseError('Cannot upload file to %s' % content_url,
                                response)

    def list_path(self, path):
        """Lists a directory or check a file status. Returns an instance
        of DirectoryListing."""
        r = self.make_call(self.client.get, path, self.metadata)
        if r.status_code == 200:
            json = r.json()
            return DirectoryListing(
                statuses=json["content"])
        elif r.status_code == 404:
            return DirectoryListing(exists=False)
        else:
            raise ResponseError('Cannot list directory %s' % path, r)

    def get_file(self, remote_path, out_file):
        response = self.make_call(self.client.get, remote_path, self.content,
                                  stream=True)
        if response.status_code == 404:
            raise ResponseError('File %s does not exist' % remote_path,
                                response)
        if response.status_code != 200:
            raise ResponseError('Cannot download file %s' % remote_path,
                                response)
        buf = response.raw.read(BUFFER_SIZE)
        written = 0
        while len(buf) > 0:
            written += len(buf)
            out_file.write(buf)
            buf = response.raw.read(BUFFER_SIZE)
        return written

    def chmod(self, remote_path, permissions):
        if not re.match('^[0-7]{3}$', permissions):
            raise OperationError('%s are not valid permissions' % permissions)
        chmod_body = json.dumps({
            "action": "chmod",
            "permissions": permissions
        })
        r = self.make_call(self.client.post, remote_path,
                           self.metadata, body=chmod_body)
        if r.status_code != 204:
            raise ResponseError(
                'Cannot change permissions on path %s' % remote_path, r)

    def delete_path(self, path, recursive=False):
        r = self.make_call(self.client.delete, path, self.metadata,
                           params=dict(recursive=str(recursive).lower()))
        if r.status_code != 204:
            raise ResponseError('Cannot delete path %s' % path, r)

    def make_call(self, method, path, type, **kwargs):
        """Translates a simple path to the related Infinity HTTP URL."""
        if path.startswith("/"):
            rel_path = path[1:]
        else:
            rel_path = path
        processedPath = urljoin(
            'http://' + urlparse(self.infinity_uri).netloc,
            '/infinityfs/v1/%s/user/%s/%s' % (type, self.username, rel_path))
        return method(processedPath,
                      auth=(self.api_key, self.api_secret), **kwargs)

