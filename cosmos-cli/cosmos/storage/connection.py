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
import os

from cosmos.common.cosmos_requests import CosmosRequests
from cosmos.common.exceptions import OperationError, ResponseError
from cosmos.common.paths import PathTypes
from cosmos.common.routes import Routes
from cosmos.common.version import assert_supported_version
from cosmos.storage.infinity import InfinityClient


def connect(api_key, api_secret, api_url):
    """Connect with the persistent storage service.

    Exceptions thrown:
        UnsupportedApiVersionException  if api_url points to a unsupported API
        ResponseException               if connection request fails
    """
    routes = Routes(api_url)
    assert_supported_version(routes.api_version)
    response = CosmosRequests((api_key, api_secret)).get(routes.storage)
    if response.status_code != 200:
        raise ResponseError("Cannot get WebHDFS details",
                            response)
    details = response.json()
    client = InfinityClient(details["location"], details["user"], api_key, api_secret)
    return StorageConnection(client)


class StorageConnection(object):
    """A connection with the persistent storage service"""

    def __init__(self, infinity_client):
        self.__client = infinity_client

    def upload_file(self, local_file, remote_path):
        """Upload an open file to the persistent storage.

        local_file must be an open file or stream supporting a name attribute.

        If remote_path exists as a directory or ends in a trailing slash, the
        file will be uploaded as a child file of the remote directory. Otherwise
        it will be uploaded and renamed at the same time.  The remote path of
        the upload is returned in any case.
        """
        target_path = self._upload_target_path(
            remote_path, os.path.split(local_file.name)[-1])
        self.__client.put_file(local_file, target_path)
        return target_path

    def _upload_target_path(self, remote_path, local_file_name):
        path_listing = self.__client.list_path(remote_path)
        if path_listing is None:
            if remote_path.endswith('/'):
                raise OperationError("Directory %s does not exist" % remote_path)
            else:
                parent_path = os.path.dirname(remote_path)
                parent_listing = self.__client.list_path(parent_path)
                if parent_listing is None:
                    raise OperationError("Parent directory %s does not exist" % parent_path)
                else:
                    target_path = remote_path
        else:
            StorageConnection._require_directory(remote_path, path_listing)
            target_path = os.path.join(remote_path, local_file_name)
        return target_path

    @staticmethod
    def _require_directory(path, path_listing):
        if path_listing.path_type == PathTypes.FILE:
            raise OperationError("Path %s already exists" % path)
        if path_listing.path_type != PathTypes.DIRECTORY:
            raise OperationError("Path %s is not a directory" % path)

    def upload_filename(self, local_filename, remote_path):
        """Upload a local file given by path.

        See upload_file to learn about the detailed behavior when remote_path
        ends with trailing slashes or exists on the remote end.
        """
        return self.upload_file(open(local_filename, 'rb'), remote_path)

    def list_path(self, path):
        """Lists a directory or check a file status. Returns a directory
        listing that you can iterate for the status objects (as defined in
        http://hadoop.apache.org/docs/r1.0.4/webhdfs.html#FileStatus).

        Directory existence can be check with the `exists` attribute of
        the returned listing.
        """
        return self.__client.list_path(path)

    def download_to_file(self, remote_path, out_file):
        """Download a file from the persistent storage to `out_file`, an open
        output file. The number of downloaded bytes is returned.
        """
        return self.__client.get_file(remote_path, out_file)

    def download_to_filename(self, remote_path, local_path):
        """Download a file from the persistent storage to the local filesystem.

        If the local path ends with trailing slash or is a directory the file is
        downloaded as a file within `local_path`. Otherwise, `local_path` is
        used as destination filename.

        A tuple of the destination path and the downloaded bytes is returned.
        """
        if local_path.endswith('/') or os.path.isdir(local_path):
            remote_filename = os.path.split(remote_path)[-1]
            target_path = os.path.join(local_path, remote_filename)
        else:
            target_path = local_path
        if os.path.isfile(target_path):
            raise OperationError("Local file already exists")
        with open(target_path, "wb") as out_file:
            size = self.download_to_file(remote_path, out_file)
        return (target_path, size)

    def delete_path(self, path, recursive):
        """Delete a file of the persistent storage."""
        return self.__client.delete_path(path, recursive)

    def chmod(self, path, permissions):
        """Changes file or directory permissions."""
        return self.__client.chmod(path, permissions)

