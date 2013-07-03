===============================
Cosmos: a CLI gate to the stars
===============================

This script allows you to create, list, see the details of Cosmos clusters.

Install Cosmos CLI from sources
===============================

The target directory will be determined by your Python installation. Usually,
it will bundle an egg into the 'site-packages' directory, and a script to the
same directory where the 'python' executable is found.  You must have `python`
and `setuptools` installed in your system.

Install using `buildout`::

    $ python ./bootstrap.py
    $ bin/buildout
    $ sudo bin/buildout setup . install

If you would like to uninstall later, use::

    $ sudo bin/buildout setup . install --record installs.txt
    $ cat installs.txt | xargs rm -rf

Install Cosmos CLI EGG
======================

To generate a redistributable archive by means of `setuptools`::

    $ bin/buildout setup . bdist_egg

This will create a file named `dist/cosmos-<cosmos version>-<python version>.egg`
tha can be downloaded and installed on any system with `python` and
`setuptools` as follows::

    $ easy_install cosmos-<version>.egg

Or directly from the network::

    $ easy_install http://location/path/cosmos-<version>.egg

You can also create a SNAPSHOT version as follows::

    $ bin/buildout setup . egg_info --tag-build=.dev`date +%G%m%d-%H%M%S` \
                           bdist_egg rotate --match=.egg --keep=10

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
