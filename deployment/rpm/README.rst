=====================================
How to create the Cosmos RPM packages
=====================================

A working `rpmbuild` command is a dependency for building these packages so
it must be installed.  The Darwin port of the `rpm` utilities is unable to
cross-build the Cosmos packages so the recommended environment for building
them is Linux.

Cosmos RPM
==========

In order to create the Cosmos RPM, the following commands need to be run:

- From the root directory:

  - `mvn clean install`
  - `sbt "set retrieveManaged in ThisBuild := true" clean test package`
  - `sbt "project cosmos-api" dist`

- From the `cosmos-cli` directory:
  - `python2.7 ./bootstrap.py`
  - `bin/buildout setup . bdist_egg`

- From the `rpm` directory:

  - `mvn clean install`


Infinityfs RPM
==============

- You can create the RPM by typing from the root directory:

  - `sbt "project infinityfs" clean dist`

- Alternatively, you can create the RPM and upload it by:

  - `sbt "project infinityfs" rpm:publish`

For the latter command you will need to have the credentials to access Nexus at
`~/.ivy2/.credentials` with the following format::

    realm=Sonatype Nexus Repository Manager
    host=cosmos10.hi.inet
    user=your_username
    password=your_password

