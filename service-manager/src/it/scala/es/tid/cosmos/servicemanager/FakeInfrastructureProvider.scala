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

package es.tid.cosmos.servicemanager

import scala.concurrent._
import scala.concurrent.ExecutionContext.Implicits.global
import scala.util.Try

import es.tid.cosmos.platform.ial._
import es.tid.cosmos.platform.ial.MachineState

class FakeInfrastructureProvider extends InfrastructureProvider {

  override def createMachines(
      namePrefix: String,
      profile: MachineProfile.Value,
      count: Int): Future[Seq[MachineState]] = Future.successful(
    for (index <- 1 to count) yield (buildMachineState(index, profile)))

  private def buildMachineState(index: Int, profile: MachineProfile.Value) =
    new MachineState(
      id = new Id[MachineState](s"cosmosLocalID$index"),
      name = "cosmos.local",
      profile = profile,
      status = MachineStatus.Running,
      hostname = s"cosmos.local$index",
      ipAddress = s"192.168.50.${index + 4}")

  def releaseMachines(machines: Seq[MachineState]): Future[Unit] = Future.successful()

  val rootSshKey: String = "FIXME"
}
