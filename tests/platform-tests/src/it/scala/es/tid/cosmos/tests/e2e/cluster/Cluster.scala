/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

  val shared: Boolean

  def id: String

  def describe(executedBy: User = owner): JValue

  def isListed(executedBy: User = owner): Boolean

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
    def apply(
        clusterSize: Int,
        owner: User,
        services: Seq[String] = Seq.empty,
        shared: Boolean = false)(implicit info: Informer): Cluster
  }

  def apply(
      clusterSize: Int,
      owner: User,
      services: Seq[String] = Seq.empty,
      shared: Boolean = false)(implicit info: Informer): Cluster =
    CommandLineManagedCluster(clusterSize, owner, services)
}
