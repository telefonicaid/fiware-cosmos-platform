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
import json
import requests

from cosmos.common.exceptions import ResponseError
from cosmos.common.routes import Routes


class Protocol(object):
    """An object that encapsulates the operations of the Cosmos
    platform RESTful API"""

    def __init__(self, api_url, credentials, client=requests):
        self.__credentials = credentials
        self.__routes = Routes(api_url)
        self.__client = client

    def get_clusters(self):
        """Obtain the list of clusters and return it in JSON format"""
        r = self.__client.get(self.__routes.clusters(),
                              auth=self.__credentials)
        if r.status_code != 200:
            raise ResponseError("Cannot list clusters", r)
        return r.json()

    def get_cluster_details(self, cluster_id):
        """Obtain the details of a cluster and return them in JSON format"""
        r = self.__client.get(self.__routes.cluster(cluster_id),
                              auth=self.__credentials)
        if r.status_code != 200:
            raise ResponseError(
                "Cannot get details for %s" % cluster_id, r)
        return r.json()

    def create_cluster(self, cluster_name, cluster_size):
        """Send a request to create a new cluster and return the response
        in JSON format"""
        body = json.dumps({ "name" : cluster_name, "size" : cluster_size })
        r = self.__client.post(self.__routes.clusters(),
                               body,
                               auth=self.__credentials)
        if r.status_code != 201:
            raise ResponseError("Cluster creation problem", r)
        return r.json()

    def terminate_cluster(self, cluster_id):
        """Request the termination of a cluster"""
        r = self.__client.post(
            self.__routes.cluster(cluster_id, action="terminate"),
            auth=self.__credentials)
        if r.status_code != 200:
            raise ResponseError("Cannot terminate cluster %s" % cluster_id, r)
        return r.json()
