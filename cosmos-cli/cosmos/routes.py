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

class Routes(object):
    """Resource URLs factory"""

    def __init__(self, base_url):
        self.base_url = base_url
        if not self.base_url.endswith("/"):
            self.base_url = self.base_url + "/"

    def clusters(self):
        """Construct the clusters resource URL.

        >>> Routes('http://foo/base').clusters()
        'http://foo/base/cluster'
        """
        return self.base_url + "cluster"

    def cluster(self, cluster_id, action=None):
        """Construct a cluster resource URL.

        >>> Routes('http://foo/base').cluster('37')
        'http://foo/base/cluster/37'

        If you need to perform a concrete action use the optional argument action:

        >>> Routes('http://foo/base').cluster('37', action="terminate")
        'http://foo/base/cluster/37/terminate'
        """
        parts = [self.clusters(), cluster_id]
        if action:
            parts.append(action)
        return "/".join(parts)

