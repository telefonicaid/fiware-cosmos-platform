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

import java.util.UUID

import org.squeryl._
import org.squeryl.PrimitiveTypeMode._

import es.tid.cosmos.platform.common._
import es.tid.cosmos.platform.ial.{MachineState, MachineStatus, MachineProfile, Id}

/**
 * A class used to maintain persistent information of machines.
 */
class Machine(var machineId: UUID,
              var name: String,
              var available: Boolean,
              var profile: MachineProfile.Value,
              var status: MachineStatus.Value,
              var hostname: String,
              var ipAddress: String) extends KeyedEntity[Long] {

  override val id: Long = 0

  override def toString: String =
      s"{ name: '$name', available: $available, profile: '$profile', " +
          s"status: '$status', hostname:'$hostname', ipAddress: '$ipAddress' }"

  def this() = this(
    UUID.randomUUID(), "unknown", false, MachineProfile.G1_COMPUTE, MachineStatus.Provisioning, "", "")

  /**
   * Transform this DB oriented data object into machine state.
   */
  def toMachineState: MachineState = {
      new MachineState(Id(machineId), name, profile, status, hostname, ipAddress)
  }

  override def equals(state: Any) = state match {
    case st: MachineState => {
      (machineId.toString == st.id.toString) &&
      (name == st.name) && (profile == st.profile) && (status == st.status) &&
      (hostname == st.hostname) && (ipAddress == st.ipAddress)
    }
    case _ => false
  }
}

object Machine {
  def apply(name: String,
            available: Boolean,
            profile: MachineProfile.Value,
            status: MachineStatus.Value,
            hostname: String,
            ipAddress: String) = new Machine(UUID.randomUUID(), name, available, profile, status, hostname, ipAddress)
}

object InfraDb extends Schema {
    val machines = table[Machine]
}

/**
 * A data access object implementation for SQL database.
 *
 * @author apv
 */
class SqlServerPoolDao(val db: SqlDatabase) extends ServerPoolDao {

  import InfraDb._

  def availableMachinesWith(f: MachineState => Boolean): Seq[MachineState] =
    transaction(db.newSession) {
      from(machines)(s => where(s.available === true) select(s))
        .map(_.toMachineState).toArray.filter(f)
    }

  def machine(machineId: Id[MachineState]): Option[MachineState] =
    machineInternal(machineId).map(_.toMachineState)

  def setMachineAvailability(
      machineId: Id[MachineState], available: Boolean): Option[MachineState] =
    modifyMachineInternal(machineId, m => m.available = available)

  def setMachineName(machineId: Id[MachineState], name: String): Option[MachineState] =
    modifyMachineInternal(machineId, m => m.name = name)

  def machineInternal(machineId: Id[MachineState]): Option[Machine] = transaction(db.newSession) {
      from(machines)(s => where(s.machineId.toString === machineId.toString) select(s)).headOption
  }

  def modifyMachineInternal(
      machineId: Id[MachineState], mod: Machine => Unit): Option[MachineState] = {
    machineInternal(machineId) match {
      case Some(m) => transaction(db.newSession) {
        mod(m)
        machines.update(m)
        Some(m.toMachineState)
      }
      case None => None
    }
  }
}

/**
 * A server pool DAO supported by a MySQL database.
 *
 * @param c connection details of MySQL server
 */
class MySqlServerPoolDao(c: MySqlConnDetails) extends SqlServerPoolDao(new MySqlDatabase(c))
