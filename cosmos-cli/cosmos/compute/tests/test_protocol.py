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
from os import path

import cosmos.common.cosmos_requests as cosmos_requests
from cosmos.common.exceptions import ResponseError
from cosmos.common.tests.util import mock_response
from cosmos.compute.protocol import Protocol


class ProtocolTest(unittest.TestCase):

    def setUp(self):
        self.api_url = 'http://localhost/api/v1'
        self.auth = ('key', 'secret')

    def __create_proto(self):
        return Protocol(self.api_url, self.auth)

    def assert_called_once_with(self, mock, *args, **keywords):
        newkeywords = keywords.copy()
        newkeywords.update({
            'auth': self.auth,
            'verify': path.join(
                path.dirname(path.realpath(cosmos_requests.__file__)),
                'cacerts.pem')})
        mock.assert_called_once_with(*args, **newkeywords)

    @patch('requests.get')
    def test_list_clusters(self, getMock):
        getMock.return_value = mock_response(status_code=200, json={
            'clusters': [
                { 'id': '1', 'name': 'cluster1', 'state': 'ready' },
                { 'id': '2', 'name': 'cluster2', 'state': 'terminated' }
            ]
        })
        rep = self.__create_proto().get_clusters()
        clusters = rep['clusters']

        self.assert_called_once_with(getMock, self.api_url + '/cluster')

        self.assertEquals(len(clusters), 2)
        self.assertEquals(clusters[0]['id'], '1')
        self.assertEquals(clusters[0]['name'], 'cluster1')
        self.assertEquals(clusters[0]['state'], 'ready')
        self.assertEquals(clusters[1]['id'], '2')
        self.assertEquals(clusters[1]['name'], 'cluster2')
        self.assertEquals(clusters[1]['state'], 'terminated')

    @patch('requests.get')
    def test_list_clusters_fail(self, getMock):
        getMock.return_value = mock_response(status_code=500, json={
            'error': 'request failed due to server error'
        })
        with self.assertRaises(ResponseError):
            self.__create_proto().get_clusters()

    @patch('requests.get')
    def test_get_cluster_details(self, getMock):
        getMock.return_value = mock_response(status_code=200, json={
            'id': '1',
            'name': 'cluster1',
            'state': 'ready'
        })
        rep = self.__create_proto().get_cluster_details('1')

        self.assert_called_once_with(getMock, self.api_url + '/cluster/1')

        self.assertEquals(rep['id'], '1')
        self.assertEquals(rep['name'], 'cluster1')
        self.assertEquals(rep['state'], 'ready')

    @patch('requests.get')
    def test_get_cluster_details_fail(self, getMock):
        getMock.return_value = mock_response(status_code=500, json={
            'error': 'request failed due to server error'
        })
        with self.assertRaises(ResponseError):
            self.__create_proto().get_cluster_details('1')

    @patch('requests.post')
    def test_create_cluster(self, postMock):
        postMock.return_value = mock_response(status_code=201, json={
            'id': '1',
            'name': 'cluster1',
            'state': 'ready'
        })
        rep = self.__create_proto().create_cluster('cluster1', 2, ['FOO'])

        expected_body = json.dumps({ 'name' : 'cluster1', 'size' : 2,
            'optionalServices': ['FOO'] })
        self.assert_called_once_with(postMock, self.api_url + '/cluster',
                                     expected_body)
        self.assertEquals(rep['id'], '1')
        self.assertEquals(rep['name'], 'cluster1')
        self.assertEquals(rep['state'], 'ready')

    @patch('requests.post')
    def test_create_cluster_fail(self, postMock):
        postMock.return_value = mock_response(status_code=500, json={
            'error': 'request failed due to server error'
        })
        with self.assertRaises(ResponseError):
            self.__create_proto().create_cluster('cluster1', 2, ['FOO', 'BAR'])

    @patch('requests.post')
    def test_terminate_cluster(self, postMock):
        postMock.return_value = mock_response(status_code=200, json={
            'message': 'termination accepted'
        })
        rep = self.__create_proto().terminate_cluster('1')

        self.assert_called_once_with(postMock,
                                     self.api_url + '/cluster/1/terminate')

        self.assertEquals(rep['message'], 'termination accepted')

    @patch('requests.post')
    def test_terminate_cluster_fail(self, postMock):
        postMock.return_value = mock_response(status_code=500, json={
            'error': 'request failed due to server error'
        })
        with self.assertRaises(ResponseError):
            self.__create_proto().terminate_cluster('1')

    @patch('requests.get')
    def test_list_services(self, getMock):
        result = ['PIG', 'OOZIE']
        getMock.return_value = mock_response(status_code=200,
            json=result)
        rep = self.__create_proto().list_services()

        self.assert_called_once_with(getMock, self.api_url + '/services')
        self.assertEquals(rep, result)

    @patch('requests.get')
    def test_list_services_fail(self, getMock):
        getMock.return_value = mock_response(status_code=404)
        rep = self.__create_proto().list_services()

        self.assert_called_once_with(getMock, self.api_url + '/services')
        self.assertEquals(rep, [])

    @patch('requests.post')
    def test_add_user_to_cluster(self, postMock):
        postMock.return_value = mock_response(status_code=200, json={
            'message': 'user added successfully'
        })
        rep = self.__create_proto().add_user_to_cluster('1', 'jsmith')

        expected_body = json.dumps({ 'user' : 'jsmith' })
        self.assert_called_once_with(postMock,
                                     self.api_url + '/cluster/1/add_user',
                                     expected_body)

        self.assertEquals(rep['message'], 'user added successfully')

    @patch('requests.post')
    def test_add_user_to_cluster_fail(self, postMock):
        postMock.return_value = mock_response(status_code=500, json={
            'error': 'request failed due to server error'
        })
        with self.assertRaises(ResponseError):
            self.__create_proto().add_user_to_cluster('1', 'jsmith')

    @patch('requests.post')
    def test_remove_user_from_cluster(self, postMock):
        postMock.return_value = mock_response(status_code=200, json={
            'message': 'user removed successfully'
        })
        rep = self.__create_proto().remove_user_from_cluster('1', 'jsmith')

        expected_body = json.dumps({ 'user' : 'jsmith' })
        self.assert_called_once_with(postMock,
                                     self.api_url + '/cluster/1/remove_user',
                                     expected_body)

        self.assertEquals(rep['message'], 'user removed successfully')

    @patch('requests.post')
    def test_remove_user_from_cluster_fail(self, postMock):
        postMock.return_value = mock_response(status_code=500, json={
            'error': 'request failed due to server error'
        })
        with self.assertRaises(ResponseError):
            self.__create_proto().remove_user_from_cluster('1', 'jsmith')

