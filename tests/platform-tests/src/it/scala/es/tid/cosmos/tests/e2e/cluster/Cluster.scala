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

package es.tid.cosmos.tests.e2e.cluster

import java.io.Closeable
import scala.language.postfixOps

import net.liftweb.json._
import org.scalatest.Informer

import es.tid.cosmos.tests.e2e.{User, Patience}

trait Cluster extends Closeable with Patience {

  protected implicit val Formats = net.liftweb.json.DefaultFormats

  val owner: User

  def id: String

  def addUser(clusterUser: String, executedBy: User = owner): Int

  def describe(executedBy: User = owner): JValue

  def isListed(executedBy: User = owner): Boolean

  def removeUser(clusterUser: String, executedBy: User = owner): Int

  def terminate(executedBy: User = owner)

  /** Upload a file to the cluster master using SCP */
  def scp(
      localFile: String,
      remotePath: String = "",
      recursive: Boolean = false,
      executedBy: User = owner)

  /** Executes a command on the master node through SSH.
    * An exception is thrown if the command has nonzero return status.
    */
  def sshCommand(command: String, executedBy: User = owner)

  def state(executedBy: User = owner): Option[String] =
    (describe(executedBy) \ "state").extractOpt[String]

  def users(executedBy: User = owner): Seq[String] =
    (describe(executedBy) \ "users").toOpt match {
      case Some(users) =>
        for {
          JObject(fields) <- users
          JField("username", JString(handle)) <- fields
        } yield handle
      case None => Seq.empty
    }

  def ensureState(expectedState: String, executedBy: User = owner) {
    eventually {
      assert(
        state(executedBy) == Some(expectedState),
        s"ExpectedState [${Some(expectedState)}] not reached. Actual: [${state(executedBy)}]." +
          s"Cluster info:\n${pretty(render(describe(executedBy)))}"
      )
    }
  }

  override def close() = {
    terminate()
    ensureState("terminated")
  }
}

object Cluster {

  trait Factory {
    def apply(clusterSize: Int, owner: User, services: Seq[String] = Seq.empty)
             (implicit info: Informer): Cluster
  }

  def apply(clusterSize: Int, owner: User, services: Seq[String] = Seq.empty)
           (implicit info: Informer): Cluster =
    CommandLineManagedCluster(clusterSize, owner, services)
}
