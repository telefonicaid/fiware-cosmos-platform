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

    def create_cluster(self, cluster_name, cluster_size, services):
        """Create a new cluster"""
        json = self.__proto.create_cluster(cluster_name, cluster_size, services)
        return Cluster(self.__proto, json)

    def list_services(self):
        """Get the list of optional services that can de installed in a
        cluster"""
        return self.__proto.list_services()
