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
import requests

from cosmos.common.routes import Routes


class Machine(object):
    """The metadata of a cluster machine"""

    def __init__(self, json):
        self.hostname = json["hostname"]
        self.ip_address = json["ipAddress"]


class Cluster(object):
    """The metadata of a Cosmos cluster"""

    def __init__(self, proto, json):
        self.__proto = proto
        self.id = json["id"]
        self.name = json["name"]
        self.href = json["href"]
        self.__master = None
        self.__slaves = []

    @property
    def state(self):
        self.__load_details()
        return self.__state

    @property
    def state_desc(self):
        self.__load_details()
        return self.__state_desc

    @property
    def master(self):
        """Obtain information of master machine of this cluster"""
        self.__load_details()
        return self.__master

    @property
    def slaves(self):
        """Obtain information of slave machines of this cluster"""
        self.__load_details()
        return self.__slaves

    def terminate(self):
        """Send a request to terminate the cluster"""
        self.__proto.terminate_cluster(self.id)

    def add_user(self, user_id):
        """Add a user to the cluster"""
        self.__proto.add_user_to_cluster(self.id, user_id)

    def remove_user(self, user_id):
        """Remove a user from the cluster"""
        self.__proto.remove_user_from_cluster(self.id, user_id)

    def __load_details(self):
        json = self.__proto.get_cluster_details(self.id)
        self.__state = json["state"]
        self.__state_desc = json["stateDescription"]
        self.__master = Machine(json["master"]) if "master" in json else None
        self.__slaves = [ Machine(slave) for slave in json["slaves"] ] \
            if "slaves" in json else []

