/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.platform.ial

import scala.concurrent.Future

import es.tid.cosmos.common.ExecutableValidation

/** An abstraction for an infrastructure provider */
trait InfrastructureProvider {
  /** Create count machines of given profile with given name prefix.
    *
    * If there are not enough infrastructure resources to satisfy the request, an error is
    * returned with `ResourceExhaustedException` wrapped.
    *
    * If the preconditions are not met, no machines will be created, returning a
    * `PreconditionsNotMetException` wrapped.
    * This allows for injecting last-moment-possible validation that will be run
    * in the sequential executor right before the allocation, removing the risk
    * for race-conditions by the check-then-set pattern.
    *
    * @param preConditions    pre-conditions that will be executed right before allocating machines.
    *                         The provider will only attempt to create machines only if there are no
    *                         validation errors
    * @param profile          the machine profile for the machines to be created.
    * @param numberOfMachines the amount of machines to be created.
    * @param bootstrapAction  action to be performed on every host just after it starts running and
    *                         to be finished to consider the provision successful
    * @return                 a future which after success provides the sequence of newly created
    *                         machines
    */
  def createMachines(
      preConditions: ExecutableValidation,
      profile: MachineProfile.Value,
      numberOfMachines: Int,
      bootstrapAction: MachineState => Future[Unit]): Future[Seq[MachineState]]

  /** Get the machines already in use for the given host names.
    *
    * @param hostNames the host names of the machines to look for
    * @return the future of the machine list corresponding to the given host names
    */
  def assignedMachines(hostNames: Seq[String]): Future[Seq[MachineState]]

  /** Release the machines so that its resources can be reused in further createMachine requests
    *
    * @param machines The set of machines to be released.
    * @return         a future which terminates once the release has completed
    */
  def releaseMachines(machines: Seq[MachineState]): Future[Unit]

  /** The ssh key that enables root access to the machines */
  val rootPrivateSshKey: String

  def availableMachineCount(profile: MachineProfile.Value): Future[Int]

  /** Get the total number of machines managed by this provider.
    * It receives a filter to allow selecting machines of specific profiles.
    *
    * @param profileFilter the machine profile filter
    * @return              the total number of managed machines regardless of their state and usage.
    */
  def machinePoolCount(profileFilter: MachineProfile.Value => Boolean): Int
}
