# -*- coding: utf-8 -*-
#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
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
