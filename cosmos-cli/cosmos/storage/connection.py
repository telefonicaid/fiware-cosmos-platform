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
import requests


from cosmos.common.exceptions import (ResponseError,
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


