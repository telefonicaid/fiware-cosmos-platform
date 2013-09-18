WebHDFS testbed for cosmos-cli
==============================

This tesbed allow for development and testing of Cosmos CLI WebHDFS features
without the need of a complete environment.

Enjoy!

Requirements
------------

 * Vagrant
 * Virtual Box
 * Including the following names in your `/etc/hosts` file:

    192.168.10.10 master
    192.168.10.21 slave1
    192.168.10.22 slave2

Usage
-----

Go to the testbed directory and:

    $ vagrant up
    $ vagrant ssh master
    master> sudo /opt/hadoop-1.1.2/bin/hadoop namenode -format
    master> sudo /opt/hadoop-1.1.2/bin/start-all.sh
     ... (write "yes" a couple of times) ...
    master> CTRL-D

You can check the HDFS is up an runngin by visiting its
[webmin page](http://192.168.10.10:50070/) or checking with `vagrant status`
that virtual machines `master`, `slave1` and `slave2` are running.

The `cosmosrc` file in the same directory is pre-configured for using the
mock interface. You can use it by just:

    $ bin/cosmos -c /path/to/testbed/cosmosrc ls /

When you are done
-----------------

If you don't need the VM's for a while, you can suspend and restore them in
matter of seconds by the commands:

    $ vagrant suspend
    $ vagrant resume

To release the machines and get the disk space back or to clean them in a
nuclear fashion:

    $ vagrant destroy

