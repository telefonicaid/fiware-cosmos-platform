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
import json
import os.path
import re
import requests
from urlparse import urlparse, urljoin

from cosmos.common.exceptions import (OperationError, ResponseError)
from cosmos.common.paths import PathTypes


SUPPORTED_VERSIONS = [1]
CHUNK_SIZE = 4096
FILE_PERMISSIONS = '640'

class DirectoryListing(object):
    """Returned when an existing directory is listed"""
    def __init__(self, statuses=None):
        self.path_type = PathTypes.DIRECTORY
        if not statuses: statuses = []
        self.statuses = statuses

    def __iter__(self):
        return self.statuses.__iter__()

    def __str__(self):
        size = len(self.statuses)
        return 'Listing(%d file%s)' % (size, '' if size == 1 else 's')

class FileMetadata(object):
    """Returned when listing an existing file path"""
    def __init__(self, the_json):
        self.path_type = PathTypes.FILE
        self.metadata = the_json


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
        base_path = os.path.dirname(remote_path)
        filename = os.path.basename(remote_path)
        create_file_body = json.dumps({
            "action": "mkfile",
            "name": filename,
            "permissions": FILE_PERMISSIONS
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
        """Lists a directory or a file. Returns an instance
        of DirectoryListing for existing directories,
         FileMetadata for existing files or
         None when path does not exist."""
        r = self.make_call(self.client.get, path, self.metadata)
        if r.status_code == 200:
            return InfinityClient._file_or_dir(r.json())
        elif r.status_code == 404:
            return None
        else:
            raise ResponseError('Cannot list path %s' % path, r)

    def get_file(self, remote_path, out_file):
        listing = self.list_path(remote_path)
        if listing is None:
            raise OperationError('File %s does not exist' % remote_path)
        if listing.path_type != PathTypes.FILE:
            raise OperationError("Path %s is not a file" % remote_path)
        content_url = listing.metadata['content']
        response = self.client.get(
            content_url, auth=(self.api_key, self.api_secret), stream=True)
        if response.status_code == 404:
            raise ResponseError('File %s does not exist' % remote_path,
                                response)
        if response.status_code != 200:
            raise ResponseError('Cannot download file %s' % remote_path,
                                response)
        written = 0
        for chunk in response.iter_content(CHUNK_SIZE):
            out_file.write(chunk)
            written += len(chunk)
        return written

    def chmod(self, remote_path, permissions):
        if not re.match('^[0-7]{3}$', permissions):
            raise OperationError('%s are not valid permissions' % permissions)
        chmod_body = json.dumps({
            "action": "chmod",
            "permissions": permissions
        })
        r = self.make_call(self.client.post, remote_path,
                           self.metadata, data=chmod_body)
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

    @staticmethod
    def _file_or_dir(the_json):
        path_type = the_json['type']
        if path_type == PathTypes.FILE:
            return FileMetadata(the_json)
        elif path_type == PathTypes.DIRECTORY:
            return DirectoryListing(statuses=the_json['content'])
        else:
            raise OperationError("Unknown resource type: %s" % path_type)

