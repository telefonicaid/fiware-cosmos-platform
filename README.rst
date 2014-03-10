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

------------------------------
Execute in Vagrant environment
------------------------------

You can deploy a sandbox Cosmos environment in local virtual machines using
Vagrant. To do so, you will need:

**To build the project**

- Python
- rpmbuild

**To run Vagrant environment**

- Vagrant 1.4+
- Virtualbox 4.2+
- About 8 GB of free memory before start provisioning the environment.

First, build the project

    $ sbt dist

*NOTE* Building requires python. It uses `python2.7` by default, but can be
overridden setting `PYTHON` environment variable, i.e.:

    $ PYTHON=/usr/bin/python2.7 sbt dist

To spin up the environment, run from this directory:

    $ vagrant up

**TODO**: current local deployment will install `cosmos-api` and `cosmos-admin`,
but Infinity and Ambari are still installed by remote RPM repos.

This will create and configure the following VMs:

 * `storage1`, `storage2`: client nodes hosting the persistent HDFS.
 * `compute1`, `compute2`: client nodes available for cluster provisioning.
 * `master`: server node with the cosmos services installed.

After a successful provision you can access any of the nodes by writing
`vagrant ssh <vmname>` and more advanced operations like suspend/restore the
environment (see `vagrant --help`).

To ease testing, you will find the local ports 80 and 443 mapped to the
`master` node. Alternatively, it can be handy to add the following contents to
`/etc/hosts` on your workstation:

    192.168.11.10 master.vagrant master
    192.168.11.21 store1
    192.168.11.22 store2
    192.168.11.23 compute1
    192.168.11.24 compute2

-------------------------------
Override remote RPMs for Ambari
-------------------------------

Vagrant will deploy by default the latest nightly build of Ambari.
This might be OK for some scenarios, but we might want to deploy a local change
we have.

If that is the case, the first thing we need to do is to generate the RPMs (in a
Linux machine for Ambari).

Next, you need to run the `createrepo` command to generate the repository
metadata. In that same directory, you can start a temporary HTTP server on port
12345 with the following command: `python -m SimpleHTTPServer 12345`

The final step is to change the `ambari::params::repo_url`
parameter in Hiera so it points to `http://<your-ip-address>:12345`. At this
point, running `vagrant up` will deploy your RPMs instead of the nightly build.

------------
Distribution
------------

Executing `sbt dist` generates a zip file containing all the deployment
descriptors (puppet) and generated RPM files.

---------------------------
About releasing & deploying
---------------------------

Deployment of Cosmos is done using Puppet. Each deployment environment (Andromeda,
Orion, etc.) has its own Hiera data which dictates where should the RPMs to install
come from. This means that the lifecycle of puppet code is coupled to that of the
Cosmos and Ambari RPMs in the RPM repositories.

When deploying to a production environment, please make sure the "release" RPM
repository in Nexus is serving the version of the platform which corresponds to the
Puppet scripts being used for deployment. You can do this by opening Nexus and
browsing to Administration -> Capabilities -> "Yum: Generate Metadata / Release".
There you can check the version being served by the repository and change it in case
that is necessary.

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
