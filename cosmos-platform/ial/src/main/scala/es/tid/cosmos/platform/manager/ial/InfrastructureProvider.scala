package es.tid.cosmos.platform.manager.ial

import scala.concurrent.Future
import scala.util.Try

/**
 * Resource exhausted exception class. This exception is thrown when there are no enough infrastructure resources
 * available to satisfy a request to the infrastructure provider.
 *
 * @param resource the type of resource which was exhausted
 * @param requested the amount of requested resources
 * @param available the amount of available resources
 */
case class ResourceExhaustedException(resource: String, requested: Int, available: Int)
  extends Exception(s"requested $requested resources of type $resource, only $available available")

/**
 * An abstraction for a infrastructure provider
 *
 * @author apv
 */
trait  InfrastructureProvider {

  /**
   * Create count machines of given profile with given name prefix. If there are no enough infrastructure resources
   * to satisfy the request, an error is returned with ResourceExhaustedException wrapped.
   *
   * @param namePrefix the prefix to prepend to the name of the newly created machines. E.g., when 3 machines are
   *                   requested with name prefix "foobar", their names would be "foobar0", "foobar1" and "foobar2".
   * @param profile the machine profile for the machines to be created.
   * @param count the amount of machines to be created.
   * @return a sequence of futures, each one representing the instantiation process of each machine, wrapped by
   *         a Try object.
   */
  def createMachines(namePrefix: String, profile: MachineProfile.Value, count: Int): Try[Seq[Future[MachineState]]]
}
