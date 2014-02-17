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

package es.tid.cosmos.api.mocks

import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import es.tid.cosmos.common.ExecutableValidation
import es.tid.cosmos.platform.ial._
import es.tid.cosmos.platform.ial.MachineProfile._
import es.tid.cosmos.platform.ial.MachineProfile
import es.tid.cosmos.platform.ial.MachineState

/** Mocked IAL that simulates a number of machines being managed
  *
  * @constructor
  * @param totalMachines  Number of simulated machines per profile
  */
class MockInfrastructureProvider(
    totalMachines: Map[MachineProfile, Int] = Map(G1Compute -> 100, HdfsMaster -> 1, HdfsSlave -> 4)
  ) extends InfrastructureProvider {

  private class Machine(name: String, val profile: MachineProfile) {
    val id = Id(name)
    val hostname = s"$name.example.com"
    var inUse: Boolean = false
    val machineState = MachineState(id, name, profile, MachineStatus.Running, hostname, "0.0.0.0")
  }

  private val machines: Seq[Machine] = totalMachines.flatMap {
    case (profile, total) => (1 to total).map(idx => new Machine(s"$profile-$idx", profile))
  }.toSeq

  override def createMachines(
      preConditions: ExecutableValidation,
      profile: MachineProfile,
      numberOfMachines: Int,
      bootstrapAction: (MachineState) => Future[Unit]): Future[Seq[MachineState]] = Future {
    machines.synchronized {
      require(preConditions().isSuccess, "Preconditions failed")
      val selectedMachines =
        machines.filter(m => !m.inUse && m.profile == profile).take(numberOfMachines)
      require(selectedMachines.size == numberOfMachines, "Not enough machines")
      selectedMachines.foreach { machine =>
        machine.inUse = true
        bootstrapAction(machine.machineState)
      }
      machines.map(_.machineState)
    }
  }

  override def assignedMachines(hostNames: Seq[String]): Future[Seq[MachineState]] = ???

  override def releaseMachines(machinesToRelease: Seq[MachineState]): Future[Unit] = Future {
    machines.synchronized {
      machinesToRelease.foreach { m =>
        machines.find(_.id == m.id).foreach(_.inUse = false)
      }
    }
  }

  override val rootPrivateSshKey: String = "ssh-rsa XXXXXX foo@example.com"

  override def availableMachineCount(profile: MachineProfile.Value): Future[Int] = Future {
    machines.synchronized {
      machines.count(m => !m.inUse && m.profile == profile)
    }
  }

  override def machinePoolCount(profileFilter: MachineProfile => Boolean): Int =
    machines.synchronized {
      machines.count(m => profileFilter(m.profile))
    }
}

object MockInfrastructureProvider {
  trait Component extends InfrastructureProviderComponent {
    override def infrastructureProvider: InfrastructureProvider = new MockInfrastructureProvider()
  }
}
