package es.tid.cosmos.platform.manager.ial

import scala.concurrent.Future
import scala.util.Try

case class ResourceExhaustedException(requested: Int, available: Int)
  extends Exception(s"requested $requested resources, only $available available")

/**
 * An abstraction for a infrastructure provider
 *
 * @author apv
 */
trait  InfrastructureProvider {

    def createMachines(
            namePrefix: String,
            profile: MachineProfile.Value,
            count: Int): Try[Seq[Future[MachineState]]]
}
