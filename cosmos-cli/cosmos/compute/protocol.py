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

from cosmos.common.cosmos_requests import CosmosRequests
from cosmos.common.exceptions import ResponseError
from cosmos.common.routes import Routes
from cosmos.common.version import assert_supported_version


class Protocol(object):
    """An object that encapsulates the operations of the Cosmos
    platform RESTful API"""

    def __init__(self, api_url, credentials):
        self.__routes = Routes(api_url)
        assert_supported_version(self.__routes.api_version)
        self.__client = CosmosRequests(credentials)

    def get_clusters(self):
        """Obtain the list of clusters and return it in JSON format"""
        r = self.__client.get(self.__routes.clusters())
        if r.status_code != 200:
            raise ResponseError("Cannot list clusters", r)
        return r.json()

    def get_cluster_details(self, cluster_id):
        """Obtain the details of a cluster and return them in JSON format"""
        r = self.__client.get(self.__routes.cluster(cluster_id))
        if r.status_code != 200:
            raise ResponseError(
                "Cannot get details for %s" % cluster_id, r)
        return r.json()

    def create_cluster(self, cluster_name, cluster_size, services, shared):
        """Send a request to create a new cluster and return the response
        in JSON format"""
        body = json.dumps({"name": cluster_name, "size": cluster_size,
                           "optionalServices": services, "shared": shared})
        r = self.__client.post(self.__routes.clusters(),
                               body)
        if r.status_code != 201:
            raise ResponseError("Cluster creation problem", r)
        return r.json()

    def terminate_cluster(self, cluster_id):
        """Request the termination of a cluster"""
        r = self.__client.post(
            self.__routes.cluster(cluster_id, action="terminate"))
        if r.status_code != 200:
            raise ResponseError("Cannot terminate cluster %s" % cluster_id, r)
        return r.json()

    def list_services(self):
        """Get the list of optional services that can de installed in a
        cluster"""
        r = self.__client.get(self.__routes.services)
        if r.status_code != 200:
            return []
        return r.json()
