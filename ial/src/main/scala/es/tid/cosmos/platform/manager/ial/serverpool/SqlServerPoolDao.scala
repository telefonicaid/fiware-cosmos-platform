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

package es.tid.cosmos.platform.manager.ial.serverpool

import java.sql.{DriverManager, Connection}
import java.util.UUID
import scala.util.Try

import org.squeryl._
import org.squeryl.adapters.MySQLAdapter
import org.squeryl.internals.DatabaseAdapter
import org.squeryl.PrimitiveTypeMode._

import es.tid.cosmos.platform.manager.ial._

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
    UUID.randomUUID(), "unknown", false, MachineProfile.M, MachineStatus.Provisioning, "", "")

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
 * A trait of an SQL databse.
 */
trait SqlDatabase extends SessionFactory {

  /**
   * Obtain the Squeryl database adapter corresponding to this SQL database.
   */
  val adapter: DatabaseAdapter

  /**
   * Create a new connection to the database.
   */
  def connect: Try[Connection]

  /**
   * Create a new Squeryl session from a connection to the database.
   */
  def newSession: Session = Session.create(connect.get, adapter)
}

/**
 * A trait of a MySQL database.
 *
 * @param host the hostname of MySQL server
 * @param port the port MySQL server is listening to
 * @param username the username used to connect to MySQL
 * @param password the password used to connect to MySQL
 * @param dbName the database to connect
 */
class MySqlDatabase(val host: String,
                    val port: Int,
                    val username: String,
                    val password: String,
                    val dbName: String) extends SqlDatabase {

  /* Initialize MySQL JDBC driver, which registers the connection chain prefix on DriverManager. */
  Class.forName("com.mysql.jdbc.Driver")

  def this(host: String, port: Int, username: String, password: String) =
    this(host, port, username, password, "")

  def connect: Try[Connection] =
      Try(DriverManager.getConnection(s"jdbc:mysql://$host:$port/$dbName", username, password))

  override val adapter = new MySQLAdapter
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
 * @param host the hostname of MySQL server
 * @param port the port MySQL server is listening to
 * @param username the username used to connect to MySQL
 * @param password the password used to connect to MySQL
 * @param dbName the database to connect
 */
class MySqlServerPoolDao(host: String, port: Int, username: String, password: String, dbName: String)
  extends SqlServerPoolDao(new MySqlDatabase(host, port, username, password, dbName))
