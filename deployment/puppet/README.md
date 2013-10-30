Cosmos Platform deployment
==========================

Automated Cosmos Platform deployment scripts via puppet.

Vagrant environment
-------------------

You can deploy a sandbox Cosmos environment in local virtual machines using
vagrant. To do so, you will need:

 * Vagrant 1.2.2. More recent versions won't perform the provision due to
   changes on the configuration DSL.
 * Virtualbox.
 * About 4 GB of free memory before start provisioning the environment.

To spin up the environment, run from this directory:

    $ vagrant up

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
