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
        self.__shared = None

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

    @property
    def shared(self):
        self.__load_details()
        return self.__shared

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
        self.__shared = json["shared"]

