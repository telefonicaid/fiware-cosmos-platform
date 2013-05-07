====================================
Big Data Platform (codenamed Cosmos)
====================================

Commit build status:  |commit_build_status|

Nightly build status: |nightly_build_status|

.. |commit_build_status| image:: http://cosmos10.hi.inet/jenkins/job/platform-commit/badge/icon
.. |nightly_build_status| image:: http://cosmos10.hi.inet/jenkins/job/platform-nightly/badge/icon

The Big Data Platform is a product that allows you to:

- run map-reduce jobs in both batch and near-real-time modes
- create and release clusters of different sizes
- create and monitor jobs
- set up external storage from Telefonica's cloud solution
- connect to get data in and out via SFTP, streaming, and HTTP
- make sure their cluster is not out of storage space
- allow the user to RDP into the head node for any additional advanced
  functionality, except if this compromises cluster performance
- provide fast, scalable consumption APIs
- support for productized analytical models and base ML techniques

=========================
Current status: Prototype
=========================

This is a work-in-progress prototype for achieving the platform's `Objective 0 <https://pdihub.hi.inet/Cosmos/cosmos-platform/wiki/Objective-0>`_.
To that end the prototype consists of 3 layers:

1. The Cosmos Public API - The platform's public REST API for managing platform clusters
2. The Service Manager - The layer responsible for provisioning Hadoop clusters and services
3. The Infrastructure Abstraction Layer (IAL) - The layer responsible for managing cluster machines

These are the layer interdependencies:

cosmos-api -> service-manager -> IAL

-------------------------
Current scope limitations
-------------------------

This is a working skeleton and as such is limited to the following assumptions:

* It assumes a cluster of 1 machine
* That machine is already available, running with Ambari installed and setup accordingly
* The Service Manager only supports deploying HDFS and MapReduce services that will be deployed in that single 1 machine of the cluster

---------------------------
Execute in development mode
---------------------------

To execute a development server go to the project root directory and enter
``sbt``::

     $ sbt
     > compile
     > project cosmos-api
     > run <<port>>

where ``<<port>>`` is the port in which the application will listen for
incoming connections. The API will have the following base path ``http://host:port/cosmos``.
