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
