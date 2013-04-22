package es.tid.cosmos.platform.manager.ial.serverpool

import scala.util.{Success, Failure, Try, Random}

import org.scalatest.{Suite, BeforeAndAfter}
import org.scalatest.matchers.{MatchResult, Matcher}
import org.squeryl.PrimitiveTypeMode._

import es.tid.cosmos.platform.manager.ial.{MachineProfile, Id}
import es.tid.cosmos.platform.manager.ial.MachineState
import es.tid.cosmos.platform.manager.ial.MachineStatus._
import java.sql.{Connection, Statement}

/**
 * A convenient trait for testing against MySQL DBMS. In order to it to work, you must:
 *
 *  - Have a mysqld running on localhost (or any other host overriding dbHost), port 3306 (or override dbPort)
 *
 *  - A username 'cosmos' (or any other overriding dbUser) with password 'cosmos' (or any other overriding dbPassword)
 *    E.g., create user 'cosmos'@'localhost' identified by 'cosmos'
 *
 *  - Select, create, drop and insert privileges on databases with 'test_' prefix to user 'cosmos'
 *    E.g., grant all on `test_%`.* to 'cosmos'@'localhost'
 *
 * @author apv
 */
trait MySqlTest extends BeforeAndAfter {

  self: Suite =>

  class ContainMachineMatcher(m: Machine) extends Matcher[Seq[MachineState]] {
    def apply(left: Seq[MachineState]) = MatchResult(
      left.exists(m == _),
      s"expected one machine state equivalent to ${m.toString} in iteration $left",
      s"expected none machine state equivalent to ${m.toString} in iteration $left"
    )
  }

  class ContainMachineWithIdMatcher(id: Id[MachineState]) extends Matcher[Seq[MachineState]] {
    def apply(left: Seq[MachineState]) = MatchResult(
      left.exists(id == _.id),
      s"expected one machine with ID $id in iteration $left",
      s"expected no machine with ID $id in iteration $left"
    )
  }

  class ContainMachinesMatcher(machines: Iterable[Machine]) extends Matcher[Seq[MachineState]] {
    def apply(left: Seq[MachineState]) = MatchResult(
      machines.forall(m => left.exists(m == _)),
      s"expected all machine states in $left are equivalent to $machines",
      s"expected all machine states in $left are not equivalent to $machines"
    )
  }

  object containMachine {
    def apply(m: Machine) = new ContainMachineMatcher(m)

    def apply(id: Id[MachineState]) = new ContainMachineWithIdMatcher(id)
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

  val cosmos02 = Machine("cosmos02", available = true, MachineProfile.S, Running, "cosmos02.hi.inet", "10.95.106.182")
  val cosmos03 = Machine("cosmos03", available = true, MachineProfile.M, Running, "cosmos03.hi.inet", "10.95.105.184")
  val cosmos04 = Machine("cosmos04", available = true, MachineProfile.M, Running, "cosmos04.hi.inet", "10.95.106.179")
  val cosmos05 = Machine("cosmos05", available = true, MachineProfile.X, Running, "cosmos05.hi.inet", "10.95.106.184")
  val cosmos06 = Machine("cosmos06", available = true, MachineProfile.XL, Running, "cosmos06.hi.inet", "10.95.110.99")
  val cosmos07 = Machine("cosmos07", available = false, MachineProfile.S, Running, "cosmos07.hi.inet", "10.95.110.203")
  val cosmos08 = Machine("cosmos08", available = false, MachineProfile.M, Running, "cosmos08.hi.inet", "10.95.111.160")
  val cosmos09 = Machine("cosmos09", available = false, MachineProfile.X, Running, "cosmos09.hi.inet", "10.95.108.75")

  val availableMachines = List(cosmos02, cosmos03, cosmos04, cosmos05, cosmos06)
  val assignedMachines = List(cosmos07, cosmos08, cosmos09)

  private def createDatabase() { updateDatabase(s"create database $dbName") }
  private def dropDatabase() { updateDatabase(s"drop database $dbName") }

  private def updateDatabase(updateQuery: String) {
    new MySqlDatabase(dbHost, dbPort, dbUser, dbPassword, "mysql").connect match {
      case Success(c) => {
        val stmt = c.createStatement()
        stmt.executeUpdate(updateQuery)
        stmt.close()
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
