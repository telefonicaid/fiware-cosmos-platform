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

