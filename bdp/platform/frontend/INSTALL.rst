Setup instructions
==================

Requirements
------------

- Python 2.7
- Thrift 0.8.0

Build instructions
------------------

- Bootstrap buildout building system. It generates an isolated environment you
  can use by invoking commands in bin/

    python bootstrap.py

- Buildout

    bin/buildout

- Generate thrift sources

    cd src/bdp_fe/jobconf/cluster/
    ./gen-code.sh

- Generate distributable package in dist/

    bin/buildout setup . bdist
