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

package es.tid.cosmos.platform.ial.serverpool

import scala.concurrent.{Future, blocking}
import scala.concurrent.ExecutionContext.Implicits.global

import es.tid.cosmos.platform.ial._

/**
 * An infrastructure provider supported by a server pool.
 */
class ServerPoolInfrastructureProvider(val dao: ServerPoolDao) extends InfrastructureProvider {

  override def createMachines(
      namePrefix: String,
      profile: MachineProfile.Value,
      count: Int,
      bootstrapAction: MachineState => Future[Unit]): Future[Seq[MachineState]] = {
    val machineIds = dao.availableMachinesWith(_.profile == profile).take(count).map(_.id)
    if (machineIds.length < count)
      Future.failed(ResourceExhaustedException(profile.toString, count, machineIds.length))
    else {
      val states: Seq[MachineState] = for {
        (id, idx) <- machineIds.zip(0 to machineIds.length)
      } yield {
        dao.setMachineAvailability(id, available = false)
        dao.setMachineName(id, s"$namePrefix$idx").get
      }
      Future.sequence(states.map(bootstrapAction)).map(_ => states)
    }
  }

  override def releaseMachines(machines: Seq[MachineState]): Future[Unit] =
    throw new UnsupportedOperationException("This method is not yet implemented")

  // TODO: fix me
  val rootPrivateSshKey: String =  ""

  // TODO: This implementation is very inefficient. We should probably rethink how the ServerPoolDao
  // works, but I'm leaving it for now since I'm not sure if this class is going to be removed
  override def availableMachineCount(profile: MachineProfile.Value): Future[Int] =
    Future { blocking {
      dao.availableMachinesWith(_.profile == profile).length
    }}
}
