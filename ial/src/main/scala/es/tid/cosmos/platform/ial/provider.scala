/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.platform.ial

import scala.concurrent.Future
import scala.util.Try

/**
 * Resource exhausted exception class. This exception is thrown when there are not enough
 * infrastructure resources available to satisfy a request to the infrastructure provider.
 *
 * @param resource the type of resource which was exhausted
 * @param requested the amount of requested resources
 * @param available the amount of available resources
 */
case class ResourceExhaustedException(resource: String, requested: Int, available: Int)
  extends Exception(s"requested $requested resources of type $resource, only $available available")

/**
 * An abstraction for an infrastructure provider
 */
trait InfrastructureProvider {
  /**
   * Create count machines of given profile with given name prefix. If there are not enough
   * infrastructure resources to satisfy the request, an error is returned with
   * ResourceExhaustedException wrapped.
   *
   * @param namePrefix the prefix to prepend to the name of the newly created machines. E.g.,
   *                   when 3 machines are requested with name prefix "foobar", their names would
   *                   be "foobar0", "foobar1" and "foobar2".
   * @param profile the machine profile for the machines to be created.
   * @param numberOfMachines the amount of machines to be created.
   * @return a sequence of futures, each one representing the instantiation process of each machine,
   *         wrapped by a Try object.
   */
  def createMachines(
    namePrefix: String,
    profile: MachineProfile.Value,
    numberOfMachines: Int): Try[Seq[Future[MachineState]]]

  /**
   * Release the machines so that its resources can be reused in further createMachine requests
   *
   * @param machines The set of machines to be released.
   * @return         a future which terminates once the release has completed
   */
  def releaseMachines(machines: MachineState*): Future[Unit]

  /**
   * The ssh key that enables root access to the machines
   */
  val rootSshKey: String
}

/**
 * This trait is aimed to implement a cake pattern to declare an artifact that requires an
 * infrastructure provider as one of its components.
 */
trait InfrastructureProviderComponent {

  /**
   * Obtain the infrastructure provider instance for this component.
   */
  def infrastructureProvider: InfrastructureProvider
}