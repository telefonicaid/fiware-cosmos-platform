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

Deploying local RPMs in Vagrant
-------------------------------

Vagrant will deploy by default the latest nightly build of Ambari and Cosmos.
This might be OK for some scenarios, but we might want to deploy a local change
we have.

If that is the case, the first thing we need to do is to generate the RPMs in a
Linux machine (please note that rpmbuild in Mac OS X has a bug and does not
work well with Maven), following the instructions in /deployment/rpm/README.md

Next, you need to run the `createrepo` command to generate the repository
metadata. In that same directory, you can start a temporary HTTP server on port
12345 with the following command: `python -m SimpleHTTPServer 12345`

The final step is to change the `cosmos::params::cosmos_repo_platform_url`
parameter in Hiera so it points to `http://<your-ip-address>:12345`. At this
point, running `vagrant up` will deploy your RPMs instead of the nightly build.
