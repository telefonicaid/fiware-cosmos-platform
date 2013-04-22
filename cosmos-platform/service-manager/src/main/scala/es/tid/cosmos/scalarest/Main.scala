package es.tid.cosmos.servicemanager.scalarest


import es.tid.cosmos.servicemanager.{ClusterId, AmbariServiceManager}
import es.tid.cosmos.servicemanager.ambari._
import es.tid.cosmos.scalarest.FakeInfrastructureProvider

object Main {

  /**
   * @param args the command line arguments
   */
  def main(args: Array[String]) {
    val server: AmbariServer = new AmbariServer("cosmos.local", 8080, "admin", "admin")
    val manager = new AmbariServiceManager(server, new FakeInfrastructureProvider)
    val cluster: ClusterId = manager.createCluster("zas", 1)
    val description = manager.describeCluster(cluster)
  }
}
