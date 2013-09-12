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
import unittest

from mock import MagicMock, patch

from cosmos.common.exceptions import ResponseError
from cosmos.common.tests.util import mock_response
from cosmos.compute.protocol import Protocol


class ProtocolTest(unittest.TestCase):

    def setUp(self):
        self.client = MagicMock()
        self.api_url = 'http://localhost/api/v1'
        self.auth = ('key', 'secret')
        self.proto = Protocol(self.api_url, self.auth, self.client)

    def test_list_clusters(self):
        self.client.get.return_value = mock_response(status_code=200, json={
            'clusters': [
                { 'id': '1', 'name': 'cluster1', 'state': 'ready' },
                { 'id': '2', 'name': 'cluster2', 'state': 'terminated' }
            ]
        })
        rep = self.proto.get_clusters()
        clusters = rep['clusters']

        self.client.get.assert_called_once_with(self.api_url + '/cluster',
                                                auth=self.auth)

        self.assertEquals(len(clusters), 2)
        self.assertEquals(clusters[0]['id'], '1')
        self.assertEquals(clusters[0]['name'], 'cluster1')
        self.assertEquals(clusters[0]['state'], 'ready')
        self.assertEquals(clusters[1]['id'], '2')
        self.assertEquals(clusters[1]['name'], 'cluster2')
        self.assertEquals(clusters[1]['state'], 'terminated')

    def test_list_clusters_fail(self):
        self.client.get.return_value = mock_response(status_code=500, json={
            'error': 'request failed due to server error'
        })
        with self.assertRaises(ResponseError):
            self.proto.get_clusters()

    def test_get_cluster_details(self):
        self.client.get.return_value = mock_response(status_code=200, json={
            'id': '1',
            'name': 'cluster1',
            'state': 'ready'
        })
        rep = self.proto.get_cluster_details('1')

        self.client.get.assert_called_once_with(self.api_url + '/cluster/1',
                                                auth=self.auth)

        self.assertEquals(rep['id'], '1')
        self.assertEquals(rep['name'], 'cluster1')
        self.assertEquals(rep['state'], 'ready')

    def test_get_cluster_details_fail(self):
        self.client.get.return_value = mock_response(status_code=500, json={
            'error': 'request failed due to server error'
        })
        with self.assertRaises(ResponseError):
            self.proto.get_cluster_details('1')

    def test_create_cluster(self):
        self.client.post.return_value = mock_response(status_code=201, json={
            'id': '1',
            'name': 'cluster1',
            'state': 'ready'
        })
        rep = self.proto.create_cluster('cluster1', 2)

        expected_body = json.dumps({ 'name' : 'cluster1', 'size' : 2 })
        self.client.post.assert_called_once_with(self.api_url + '/cluster',
                                                 expected_body,
                                                 auth=self.auth)
        self.assertEquals(rep['id'], '1')
        self.assertEquals(rep['name'], 'cluster1')
        self.assertEquals(rep['state'], 'ready')

    def test_create_cluster_fail(self):
        self.client.get.return_value = mock_response(status_code=500, json={
            'error': 'request failed due to server error'
        })
        with self.assertRaises(ResponseError):
            self.proto.create_cluster('cluster1', 2)

    def test_terminate_cluster(self):
        self.client.post.return_value = mock_response(status_code=200, json={
            'message': 'termination accepted'
        })
        rep = self.proto.terminate_cluster('1')

        self.client.post.assert_called_once_with(
            self.api_url + '/cluster/1/terminate', auth=self.auth)

        self.assertEquals(rep['message'], 'termination accepted')

    def test_terminate_cluster_fail(self):
        self.client.get.return_value = mock_response(status_code=500, json={
            'error': 'request failed due to server error'
        })
        with self.assertRaises(ResponseError):
            self.proto.terminate_cluster('1')
