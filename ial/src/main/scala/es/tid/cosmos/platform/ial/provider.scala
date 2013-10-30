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
   * @param profile the machine profile for the machines to be created.
   * @param numberOfMachines the amount of machines to be created.
   * @param bootstrapAction action to be performed on every host just after it starts running and
   *                        to be finished to consider the provision successful
   * @return a future which after success provides the sequence of newly created machines
   */
  def createMachines(
      profile: MachineProfile.Value,
      numberOfMachines: Int,
      bootstrapAction: MachineState => Future[Unit]): Future[Seq[MachineState]]

  /**
   * Get the machines already in use for the given host names.
   *
   * @param hostNames the host names of the machines to look for
   * @return the future of the machine list corresponding to the given host names
   */
  def assignedMachines(hostNames: Seq[String]): Future[Seq[MachineState]]

  /**
   * Release the machines so that its resources can be reused in further createMachine requests
   *
   * @param machines The set of machines to be released.
   * @return         a future which terminates once the release has completed
   */
  def releaseMachines(machines: Seq[MachineState]): Future[Unit]

  /**
   * The ssh key that enables root access to the machines
   */
  val rootPrivateSshKey: String

  def availableMachineCount(profile: MachineProfile.Value): Future[Int]

  /**
   * Get the total number of machines managed by this provider.
   *
   * @return the total number of managed machines regardless of their state and usage.
   */
  def machinePoolCount: Int
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
