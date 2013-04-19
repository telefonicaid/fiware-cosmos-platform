package es.tid.cosmos.servicemanager.scalarest

import dispatch._, Defaults._
import net.liftweb.json.parse
import net.liftweb.json.JsonAST.JObject
import scala.util.control.NonFatal

import es.tid.cosmos.servicemanager.ambari._
import es.tid.cosmos.servicemanager.{ClusterId, AmbariServiceManager}
import es.tid.cosmos.ila.{MachineState, InfrastructureProvider, InfrastructureProviderComponent}

object Main {

  /**
   * @param args the command line arguments
   */
  def main(args: Array[String]): Unit = {
    val server: AmbariServer = new AmbariServer("cosmos.local", 8080, "admin", "admin")
    val manager = new AmbariServiceManager(server) with Fake
    val cluster: ClusterId = manager.createCluster("zas", 1)
    val description = manager.describeCluster(cluster)
  }

  trait Fake extends InfrastructureProviderComponent {
    override val infrastructureProvider = new InfrastructureProvider{
      val machine = new MachineState {
              val name: String = "cosmos.local"
            }
      val machineFuture: Future[MachineState] = Future { machine }
      def createMachines(namePrefix: String, profile: Unit, count: Int): Seq[Future[MachineState]] = List(machineFuture)
    }
  }
}
