===============================
Cosmos: a CLI gate to the stars
===============================

This script allows you to create, list, see the details of Cosmos clusters.

Install Cosmos
==============

The target directory will be determined by your Python installation. Usually,
it will bundle an egg into the 'site-packages' directory, and a script to the
same directory where the 'python' executable is found.

Install using::

    $ python setup.py install

If you would like to uninstall later, use::

    $ python setup.py install --record installs.txt
    $ cat installs.txt | xargs rm -rf

Configure Cosmos
================

By default, your cosmos is supposed to be at http://localhost:9000/cosmos but
you can create a custom config file by::

    $ cosmos configure

Let the universe spin
=====================

- Create a cluster::

   $ cosmos create --name <name> --size <size>

- List clusters::

   $ cosmos list

- See cluster details::

   $ cosmos show <cluster id>
