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

from cosmos.compute.cluster import Cluster, Machine

def machine_json(id):
    return { 'hostname': 'machine' + id, 'ipAddress': '192.168.100.' + id }

class ClusterTest(unittest.TestCase):

    def setUp(self):
        self.proto = MagicMock()
        self.api_url = 'http://localhost/api/v1'
        self.auth = ('key', 'secret')

        self.cluster = Cluster(self.proto, {
            'id': '1',
            'name': 'cluster1',
            'href': self.api_url + '/cluster/1'
        })
        self.machine1 = machine_json('1')
        self.machine2 = machine_json('2')
        self.machine3 = machine_json('3')

        initial = {
            'id': '1',
            'name': 'cluster1',
            'href': self.api_url + '/cluster/1',
            'state': 'provisioning',
            'stateDescription': 'the cluster is provisioning',
            'shared': True
        }

        machines_are_ready = initial.copy()
        machines_are_ready.update({
            'master' : self.machine1,
            'slaves' : [ self.machine2, self.machine3 ]
        })

        cluster_is_ready = machines_are_ready.copy()
        cluster_is_ready.update({
            'state': 'running',
            'stateDescription': 'the cluster is running',
        })

        self.proto.get_cluster_details = MagicMock(side_effect=[
            initial, machines_are_ready, cluster_is_ready])

    def test_get_basic_data(self):
        self.assertEquals(self.cluster.id, '1')
        self.assertEquals(self.cluster.name, 'cluster1')
        self.assertEquals(self.cluster.href, self.api_url + '/cluster/1')

    def test_get_state(self):
        self.assertEquals(self.cluster.state, 'provisioning')
        self.assertEquals(self.cluster.state, 'provisioning')
        self.assertEquals(self.cluster.state, 'running')

    def get_state_description(self):
        self.assertEquals(self.cluster.state_desc, 'the cluster is provisioning')
        self.assertEquals(self.cluster.state_desc, 'the cluster is provisioning')
        self.assertEquals(self.cluster.state_desc, 'the cluster is running')

    def get_shared(self):
        self.assertEquals(self.cluster.shared, True)

    def get_master(self):
        self.assertEquals(self.cluster.master, None)
        self.assertEquals(self.cluster.master, Machine(self.machine1))
        self.assertEquals(self.cluster.master, Machine(self.machine1))

    def get_slaves(self):
        slaves = self.cluster.slaves
        self.assertEquals(len(slaves), 0)

        slaves = self.cluster.slaves
        self.assertEquals(len(slaves), 2)
        self.assertEquals(slaves[0], self.machine2)
        self.assertEquals(slaves[1], self.machine3)

        slaves = self.cluster.slaves
        self.assertEquals(len(slaves), 2)
        self.assertEquals(slaves[0], self.machine2)
        self.assertEquals(slaves[1], self.machine3)
