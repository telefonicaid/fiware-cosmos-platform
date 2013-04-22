======================================================
Big Data Platform (codenamed Cosmos) - Scala Prototype
======================================================

This is a work-in-progress prototype for achieving the platform's `Objective 0 <https://pdihub.hi.inet/Cosmos/cosmos-platform/wiki/Objective-0>`_.
To that end the prototype consists of 3 layers:

1. The Cosmos Public API - The platform's public REST API for managing platform clusters
2. The Service Manager - The layer responsible for provisioning Hadoop clusters and services
3. The Infrastructure Abstraction Layer (IAL) - The layer responsible for managing cluster machines

These are the layer interdependencies:

cosmos-api -> service-manager -> IAL

==========================
Current scope limitations:
==========================

This is a working skeleton and as such is limited to the following assumptions:

* It assumes a cluster of 1 machine
* That machine is already available, running with Ambari installed and setup accordingly
* The Service Manager only supports deploying HDFS and MapReduce services that will be deployed in that single 1 machine of the cluster