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
import os

import requests

from cosmos.common.exceptions import (OperationError, ResponseError,
                                      UnsupportedApiVersionException)
from cosmos.common.routes import Routes
from cosmos.storage.webhdfs import WebHdfsClient


SUPPORTED_VERSIONS = [1]


def connect(api_key, api_secret, api_url):
    """Connect with the persistent storage service.

    Exceptions thrown:
        UnsupportedApiVersionException  if api_url points to a unsupported API
        ResponseException               if connection request fails
    """
    routes = Routes(api_url)
    if not routes.api_version in SUPPORTED_VERSIONS:
        raise UnsupportedApiVersionException(routes.api_version,
                                                SUPPORTED_VERSIONS)
    response = requests.get(routes.storage, auth=(api_key, api_secret))
    if response.status_code != 200:
        raise ResponseError("Cannot get WebHDFS details",
                            response)
    details = response.json()
    client = WebHdfsClient(details["location"], details["user"])
    return StorageConnection(client)


class StorageConnection(object):
    """A connection with the persistent storage service"""

    def __init__(self, webhdfs_client):
        self.__client = webhdfs_client

    def upload_file(self, local_file, remote_path):
        """Upload an open file to the persistent storage.

        local_file must be an open file or stream supporting a name attribute.

        If remote_path exists as a directory or ends in a trailing slash, the
        file will be uploaded as a child file of the remote directory. Otherwise
        it will be uploaded and renamed at the same time.  The remote path of
        the upload is returned in any case.
        """
        remote_type = self.__client.path_type(remote_path)
        if remote_type == 'FILE':
            raise OperationError("Path %s already exists" % remote_path)
        if remote_path.endswith('/') or remote_type == 'DIRECTORY':
            target_path = os.path.join(remote_path,
                                       os.path.split(local_file.name)[-1])
        else:
            target_path = remote_path
        self.__client.put_file(local_file, target_path)
        return target_path

    def upload_filename(self, local_filename, remote_path):
        """Upload a local file given by path.

        See upload_file to learn about the detailed behavior when remote_path
        ends with trailing slashes or exists on the remote end.
        """
        return self.upload_file(open(local_filename, 'rb'), remote_path)
