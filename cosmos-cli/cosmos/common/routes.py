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
"""Routes mapping"""

import re
from urlparse import urlparse

PATH_PATTERN = re.compile('(?:/[^/]+)*/v(\d+)/?')


class Routes(object):
    """Resource URLs factory"""

    def __init__(self, base_url):
        """Initializes a Routes object and pre-computes fixed resources.

        >>> instance = Routes('http://foo/base/v1')
        >>> instance.storage
        'http://foo/base/v1/storage'
        >>> instance.profile
        'http://foo/base/v1/profile'
        >>> instance.api_version
        1

        Base urls must include a version tag or a ValueError is raised.

        >>> Routes('http://foo/unversioned')
        Traceback (most recent call last):
            ...
        ValueError: Base API url has not version tag: "http://foo/unversioned"
        """
        self.__set_base_url(base_url)
        self.info = self.base_url + "/info"
        self.storage = self.base_url + "/storage"
        self.profile = self.base_url + "/profile"
        self.services = self.base_url + "/services"

    def clusters(self):
        """Construct the clusters resource URL.

        >>> Routes('http://foo/base/v1').clusters()
        'http://foo/base/v1/cluster'
        """
        return "%s/cluster" % (self.base_url)

    def cluster(self, cluster_id, action=None):
        """Construct a cluster resource URL.

        >>> Routes('http://foo/base/v1').cluster('37')
        'http://foo/base/v1/cluster/37'

        If you need to perform a concrete action use the optional argument action:

        >>> Routes('http://foo/base/v1').cluster('37', action="terminate")
        'http://foo/base/v1/cluster/37/terminate'
        """
        parts = [self.clusters(), cluster_id]
        if action:
            parts.append(action)
        return "/".join(parts)

    def __set_base_url(self, url):
        parts = urlparse(url)
        match = PATH_PATTERN.match(parts.path)
        if not match:
            raise ValueError('Base API url has not version tag: "%s"' % url)
        self.base_url = url if not url.endswith('/') else url[:-1]
        self.api_version = int(match.group(1))
