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

import scala.util.{Success, Failure, Random}

import org.scalatest.{Suite, BeforeAndAfter}
import org.scalatest.matchers.{MatchResult, Matcher}
import org.squeryl.PrimitiveTypeMode._

import es.tid.cosmos.platform.manager.ial.{MachineProfile, Id}
import es.tid.cosmos.platform.manager.ial.MachineState
import es.tid.cosmos.platform.manager.ial.MachineStatus._

/**
 * A convenient trait for testing against MySQL DBMS. In order for it to work, you must:
 *
 * - Have a mysqld running on localhost (or any other host overriding dbHost), port 3306 (or
 *  override dbPort)
 * - A username 'cosmos' (or any other overriding dbUser) with password 'cosmos' (or any other
 *  overriding dbPassword). E.g., create user 'cosmos'@'localhost' identified by 'cosmos'
 * - Select, create, drop and insert privileges on databases with 'test_' prefix to user
 *  'cosmos'. E.g., grant all on `test_%`.* to 'cosmos'@'localhost'
 */
trait MySqlTest extends BeforeAndAfter {
  this: Suite =>

  class ContainSomeMachineMatcher(matcherDesc: String) (pred: MachineState => Boolean)
        extends Matcher[Seq[MachineState]] {
    def apply(left: Seq[MachineState]) = MatchResult(
      left.exists(pred),
      s"expected one $matcherDesc in iteration $left",
      s"expected none $matcherDesc in iteration $left")
  }

  class ContainMachinesMatcher(machines: Iterable[Machine]) extends Matcher[Seq[MachineState]] {
    def apply(left: Seq[MachineState]) = MatchResult(
      machines.forall(m => left.exists(m == _)),
      s"expected all machine states in $left are equivalent to $machines",
      s"expected all machine states in $left are not equivalent to $machines"
    )
  }

  object containMachine {
    def apply(m: Machine) = new ContainSomeMachineMatcher(
      s"state equivalent to ${m.toString}")(m == _)
    def apply(id: Id[MachineState]) = new ContainSomeMachineMatcher(
      s"machine with id $id")(id == _.id)
    def apply(name: String) = new ContainSomeMachineMatcher(
      s"machine with name $name") (_.name == name)
  }

  object containMachines {
    def apply(machines: Iterable[Machine]) = new ContainMachinesMatcher(machines)
  }

  val dbHost = "localhost"
  val dbPort = 3306
  val dbUser = "cosmos"
  val dbPassword = "cosmos"
  val dbName = s"test_${math.abs(Random.nextInt())}"
  val db = new MySqlDatabase(dbHost, dbPort, dbUser, dbPassword, dbName)
  val dao = new SqlServerPoolDao(db)

  val machines = List(
    Machine(
      "cosmos02", available = true, MachineProfile.S, Running, "cosmos02.hi.inet", "10.95.106.182"),
    Machine(
      "cosmos03", available = true, MachineProfile.M, Running, "cosmos03.hi.inet", "10.95.105.184"),
    Machine(
      "cosmos04", available = true, MachineProfile.M, Running, "cosmos04.hi.inet", "10.95.106.179"),
    Machine(
      "cosmos05", available = true, MachineProfile.X, Running, "cosmos05.hi.inet", "10.95.106.184"),
    Machine(
      "cosmos06", available = true, MachineProfile.XL, Running, "cosmos06.hi.inet", "10.95.110.99"),
    Machine(
      "cosmos07", available = false, MachineProfile.S, Running, "cosmos07.hi.inet", "10.95.110.203"),
    Machine(
      "cosmos08", available = false, MachineProfile.M, Running, "cosmos08.hi.inet", "10.95.111.160"),
    Machine(
      "cosmos09", available = false, MachineProfile.X, Running, "cosmos09.hi.inet", "10.95.108.75"))

  def availableMachines = machines.filter(m => m.available)
  def assignedMachines = machines.filter(m => !m.available)

  private def createDatabase() { updateDatabase(s"create database $dbName") }
  private def dropDatabase() { updateDatabase(s"drop database $dbName") }

  private def updateDatabase(updateQuery: String) {
    var stmt: Option[java.sql.Statement] = None
    new MySqlDatabase(dbHost, dbPort, dbUser, dbPassword).connect match {
      case Success(c) => try {
        stmt = Some(c.createStatement())
        stmt.get.executeUpdate(updateQuery)
      } finally {
        stmt.map(_.close())
        c.close()
      }
      case Failure(e) => throw e
    }
  }

  before {
    createDatabase()
    transaction(db.newSession) {
      InfraDb.create
      InfraDb.machines.insert(availableMachines ++ assignedMachines)
    }
  }

  after {
    transaction(db.newSession) {
      InfraDb.drop
    }
    dropDatabase()
  }
}
