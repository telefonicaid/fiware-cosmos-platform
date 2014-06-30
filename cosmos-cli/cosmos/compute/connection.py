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
from cosmos.compute.cluster import Cluster
from cosmos.compute.protocol import Protocol


class ComputeConnection(object):
    """A connection with the compute service. """

    def __init__(self, api_url, credentials, proto=None):
        """Create a new compute connection

        Keyword arguments:
        api_url -- the URL of the API endpoint
        credentials -- the tuple of API key and API secret
        """
        self.__proto = (Protocol(api_url, credentials)
            if proto is None else proto)

    def list_clusters(self):
        """List existing clusters"""
        json = self.__proto.get_clusters()
        clusters = json["clusters"]
        return [Cluster(self.__proto, c) for c in clusters]

    def create_cluster(self, cluster_name, cluster_size, services, shared):
        """Create a new cluster"""
        json = self.__proto.create_cluster(
            cluster_name, cluster_size, services, shared)
        return Cluster(self.__proto, json)

    def list_services(self):
        """Get the list of optional services that can de installed in a
        cluster"""
        return self.__proto.list_services()
