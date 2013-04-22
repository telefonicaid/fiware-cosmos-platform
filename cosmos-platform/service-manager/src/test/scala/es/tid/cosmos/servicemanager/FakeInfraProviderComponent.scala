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
import es.tid.cosmos.servicemanager.ila.InfrastructureProviderComponent
import es.tid.cosmos.platform.manager.ial._
import scala.util.Try

trait FakeInfraProviderComponent extends InfrastructureProviderComponent {
  val infrastructureProvider = new InfrastructureProvider {
    override def createMachines(namePrefix: String, profile: MachineProfile.Value, count: Int):
      Try[Seq[Future[MachineState]]] = count match {
      case 1 => Try(List( future { blocking { new MachineState(
              id = new Id[MachineState]("cosmosLocalID"),
              name = "cosmos.local",
              profile = MachineProfile.XS,
              status = MachineStatus.Running,
              hostname = "cosmos.local",
              ipAddress = "192.168.50.4")}}))
      case _ => throw new Error
    }
  }
}
