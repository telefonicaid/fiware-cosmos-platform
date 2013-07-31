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
import xml.etree.ElementTree as et

def parse_pom_version(pom_path):
    project_node = et.parse(pom_path)
    version_node = project_node.find(
        './mvn:version',
        namespaces=dict(mvn='http://maven.apache.org/POM/4.0.0'))
    if version_node is None:
        raise ValueError("Cannot find a project version on %s" % pom_path)
    return version_node.text
