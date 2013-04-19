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
import es.tid.cosmos.ila.{MachineState, InfrastructureProvider, InfrastructureProviderComponent}

trait FakeInfraProviderComponent extends InfrastructureProviderComponent {
  val infrastructureProvider = new InfrastructureProvider {
    override def createMachines(namePrefix: String, profile: Unit, count: Int): Seq[Future[MachineState]] = count match {
      case 1 => List( future { blocking { new MachineState {
        override val name: String = "localhost.localdomain"
      }}})
      case _ => throw new Error
    }
  }
}
