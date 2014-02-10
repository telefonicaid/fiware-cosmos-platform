========================================
End-to-end tests for the Cosmos platform
========================================

This module defines end-to-end tests for the cosmos platform. You can run the tests against
the default environment by:

- Make sure that ``cosmos`` command is available in the system path.
- ``sbt 'project platform-tests' it:test``

To run against the ``vagrant`` environment you must follow the these steps:

- Make sure that ``cosmos`` command is available in the system path.
- Provision the vagrant environment (``vagrant up`` at ``deployment/puppet/``)
- Make sure that ``master.vagrant`` points to 192.168.11.10 by editing ``/etc/hosts``
- ``sbt 'project platform-tests' -DtestConfig=vagrant-test.conf``
