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

You will also need to have `distribute` installed (e.g. by executing
`easy_install distribute`)

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

Testing
=======

You can run the Python tests with the following command::

    $ bin/test

If you also want to measure the code coverage, you can do so with::

    $ bin/coverage

Configure Cosmos
================

By default, your cosmos is supposed to be at https://cosmos.hi.inet/cosmos/v1
but you can create a custom config file by::

    $ cosmos configure

Let the universe spin
=====================

- Create a cluster::

   $ cosmos create --name <name> --size <size>

- List clusters::

   $ cosmos list

- See cluster details::

   $ cosmos show <cluster id>
