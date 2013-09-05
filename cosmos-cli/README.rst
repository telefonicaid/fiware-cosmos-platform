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
    $ bin/buildout -c dev.cfg
    $ sudo bin/buildout setup . install

The same process is executed on the Jenkins server with a different profile
(``-c jenkins.cfg``) to perform diverse static analyses.  If you would like to
uninstall it later, use::

    $ sudo bin/buildout setup . install --record installs.txt
    $ cat installs.txt | xargs rm -rf

Note that the previous commands won't work if you have a python installation
with setuptools version under 0.7. Take a look at setuptools documentation_ in
that case.

You will also need to have `distribute` installed (e.g. by executing `easy_install distribute`)

.. _documentation: https://pypi.python.org/pypi/setuptools/0.8#installing-and-using-setuptools

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

Publishing the Cosmos CLI EGG
=============================

Once a version of the component is ready to be published you can upload it to
the local PYPI proxy, colocated in the same box as the continuous integration
tools, ``cosmos10``, as follows::

    $ bin/buildout setup . egg_info --tag-build=.dev`date +%G%m%d-%H%M%S` \
                           upload -r http://cosmos10.hi.inet:8000/simple/

In order to publish a non-snapshot version you should omit the ``--tag-build``
flag and to publish to the public PYPI, the ``-r`` one.

To publish from a host other than ``cosmos10`` the ``~/.pypirc`` configuration
file should containt the server credentials. Take a look at a sample
configuration::

     [distutils]
     index-servers = pyshop

     [pyshop]
     username: jenkins
     password: jenkins
     repository: http://cosmos10.hi.inet:8000/simple/

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
