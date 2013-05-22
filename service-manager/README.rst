===================================
Expand the universe with new cosmos
===================================

ServiceManager is a module that takes care of managing Cosmos clusters and their services. It allows you to
create, get information and terminate Cosmos clusters with specific services on them. A ServiceManager can support various services the user can chose from to install and run on Cosmos clusters such as Hadoop HDFS, MapReduce and Oozie.

-------------------------
Available ServiceManagers
-------------------------
The module comes with a ServiceManager implementation that uses `Apache Ambari <http://incubator.apache.org/ambari/>`_ underneath for cluster provisioning, the ``AmbariServiceManager``.

------------------
Available Services
------------------
Currently the ``AmbariServiceManager`` supports:

1. Hadoop HDFS
2. Hadoop MapReduce
3. Apache Oozie
4. Cosmos cluster User - the principal cluster user with SSH access to it and a dedicated HDFS user directory

------------
Dependencies
------------
The ``AmbariServiceManager`` needs:

1. A **cluster provisioner** capable of provisioning clusters. This is offered by the ``AmbariServer`` which acts as a proxy to the Ambari web API.
2. An **IAL infrastructure provider** capable of provisioning machines. See `IAL <https://pdihub.hi.inet/Cosmos/cosmos-platform/tree/develop/ial/>`_ for more information.

------
Use it
------
The easy way with our predefined cake-pattern components:

``AmbariServiceManagerComponent`` offers a preconfigured ``AmbariServiceManager`` with ``AmbariServer`` as the cluster provisioner while ``ServerPoolInfrastructureProviderComponent`` offers the default IAL infrastructure provider.
::
  class YourClusterClient with AmbariServiceManagerComponent with ServerPoolInfrastructureProviderComponent {
    def braveNewCosmos = {
      val yourUser = ClusterUser("your-username", "your-public-key")
      val yourClusterId = serviceManager.createCluster(
        name = "yourCosmos", 
        clusterSize = 10,
        serviceDescriptions = serviceManager.services(yourUser)
      )
      yourClusterId
    }
  }


The traditional way
::

  val clusterProvider = new AmbariServer(
    serverUrl = "http://your-ambari-server.example.com",
    port      = "8080",
    username  = "admin",
    password  = "admin"
  )
  val infrastructureProvider = new ServerPoolInfrastructureProvider(dao) // see IAL for dao dependencies
  val serviceManager = new AmbariServiceManager(clusterProvider, infrastructureProvider)
  val yourUser = ClusterUser("your-username", "your-public-key")
  val yourClusterId = serviceManager.createCluster(
    name = "yourCosmos", 
    clusterSize = 10,
    serviceDescriptions = serviceManager.services(yourUser)
  )
