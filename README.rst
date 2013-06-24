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

---------------------------
Execute in development mode
---------------------------

To execute a development server go to the project root directory and enter
``sbt``::

     $ sbt
     > compile
     > project cosmos-api
     > run

The application will start at http://localhost:9000. To change the listen path
just edit the ``application.baseurl`` setting on the ``application.conf``
file.

========
Glossary
========

Cosmos does automatic deployment of clusters and needs to be deployed itself
--as all systems. To avoid confusion in this and other areas we define here
some terms.


- *Component*: piece of software that constitutes a part of Cosmos and gets
  deployed as part of Cosmos deployment.

- *Cosmos Public API*: the API to service the end user tools.

- *Deployment*: given a set of machines, the action of deploying a cluster
  will install and configure all requested software in those machines.

- *Head Node*: in a cluster, the Head Node is the server which holds the
  master components for all services (NameNode for HDFS, JobTracker for
  MapRed, etc.).

- *IAL*: see *Infrastructure Abstraction Layer*.

- *Infrastructure Abstraction Layer*: the software component that provides
  provisioning mechanism for the different infrastructure resources.

- *Master Node*: the server which runs all administration components like
  the IAL and the Service Manager.

- *Provision*: provisioning a set of machines implies creating and booking
  the needed resources for the specific user or cluster that initiated the
  request.

- *Service*: piece of software that gets deployed by Cosmos. E.g. mapred daemon,
  oozie...

- *Service Manager*: the software component that provides deployment
  mechanisms for the different services in the cluster.
