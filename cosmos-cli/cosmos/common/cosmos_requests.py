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
"""Requests to Cosmos"""

from functools import partial
from os import path
import requests

class CosmosRequests(object):
    """
    Authenticated requests for Cosmos with SSL cert validation.
    It wraps the calls to the requests library.
    """

    def __init__(self, credentials,
                 cert=path.join(path.dirname(path.realpath(__file__)),
                                'cacerts.pem')):
        """Initializes a CosmosRequest object."""
        self.__credentials = credentials
        self.__cert = cert
        self.get = partial(requests.get, auth=credentials, verify=cert)
        self.post = partial(requests.post, auth=credentials, verify=cert)
