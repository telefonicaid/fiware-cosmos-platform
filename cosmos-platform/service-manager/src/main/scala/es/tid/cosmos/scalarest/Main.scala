package es.tid.cosmos.servicemanager.scalarest

import dispatch._, Defaults._
import net.liftweb.json.parse
import net.liftweb.json.JsonAST.JObject
import scala.util.control.NonFatal

import es.tid.cosmos.servicemanager.ambari._
import es.tid.cosmos.servicemanager.{ClusterId, AmbariServiceManager}
import es.tid.cosmos.servicemanager.ila.InfrastructureProviderComponent
import es.tid.cosmos.platform.manager.ial._
import es.tid.cosmos.platform.manager.ial.MachineState
import scala.util.Try
import scala.concurrent.Future

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
      val machine = new MachineState(
        id = new Id[MachineState]("cosmosLocalID"),
        name = "cosmos.local",
        profile = MachineProfile.XS,
        status = MachineStatus.Running,
        hostname = "cosmos.local",
        ipAddress = "192.168.50.4")

      val machineFuture: Future[MachineState] = Future { machine }

      override def createMachines(namePrefix: String, profile: MachineProfile.Value, count: Int):
        Try[Seq[Future[MachineState]]] = Try(List(machineFuture))
    }
  }
}
