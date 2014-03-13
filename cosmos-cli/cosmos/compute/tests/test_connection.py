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
import unittest

from mock import MagicMock

from cosmos.compute.cluster import Cluster
from cosmos.compute.connection import ComputeConnection


class ComputeConnectionTest(unittest.TestCase):

    def cluster_json(self, id):
        return {
            'id': id,
            'name': 'cluster' + id,
            'href': self.api_url + '/cluster/' + id
        }

    def setUp(self):
        self.proto = MagicMock()
        self.api_url = 'http://localhost/api/v1'
        self.auth = ('key', 'secret')
        self.conn = ComputeConnection(self.api_url, self.auth, self.proto)

        self.proto.get_clusters.return_value = { 'clusters': [
            self.cluster_json('1'), self.cluster_json('2')
        ]}
        self.proto.create_cluster.return_value = self.cluster_json('3')

    def test_list_clusters(self):
        clusters = self.conn.list_clusters()
        self.assertEquals(len(clusters), 2)
        self.assertEquals(clusters[0].id, '1')
        self.assertEquals(clusters[0].name, 'cluster1')
        self.assertEquals(clusters[0].href, self.api_url + '/cluster/1')
        self.assertEquals(clusters[1].id, '2')
        self.assertEquals(clusters[1].name, 'cluster2')
        self.assertEquals(clusters[1].href, self.api_url + '/cluster/2')

    def test_list_clusters(self):
        cluster = self.conn.create_cluster('cluster3', 2, [], False)
        self.assertEquals(cluster.id, '3')
        self.assertEquals(cluster.name, 'cluster3')
        self.assertEquals(cluster.href, self.api_url + '/cluster/3')

    def test_list_services(self):
        result = ['HIVE', 'SQOOP']
        self.proto.list_services.return_value = result
        self.assertEquals(self.conn.list_services(), result)

