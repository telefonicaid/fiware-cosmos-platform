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

import scala.language.postfixOps
import scala.sys.process._

import net.liftweb.json._
import net.liftweb.json.JsonAST.{JString, JField, JObject}
import org.scalatest.Informer
import org.scalatest.verb.MustVerb
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.tests.e2e.{CommandLineMatchers, User}

class CommandLineManagedCluster(val id: String, val owner: User, val shared: Boolean)
                               (implicit info: Informer)
  extends Cluster with MustVerb with MustMatchers with CommandLineMatchers {

  private val sshFlags = "-o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no"

  override def isListed(executedBy: User = owner): Boolean =
    (s"cosmos -c ${executedBy.cosmosrcPath} list" lines_!).exists(_.contains(id))

  override def describe(executedBy: User = owner) = parse(
    s"cosmos -c ${executedBy.cosmosrcPath} show $id" !! ProcessLogger(info(_))
  )

  override def terminate(executedBy: User = owner) {
    info(s"Calling terminate on cluster $id")
    s"cosmos -c ${executedBy.cosmosrcPath} terminate $id" ! ProcessLogger(info(_))
  }

  override def scp(
      localFile: String,
      remotePath: String = "",
      recursive: Boolean = false,
      executedBy: User = owner) {
    val hostname = masterHostname(executedBy).getOrElse(fail("No master to scp to"))
    val scpFlags = if (recursive) "-r" else ""
    val command = s"scp $sshFlags $scpFlags $localFile ${executedBy.handle}@$hostname:$remotePath"
    info(s"Copying files to cluster via SCP with command: $command")
    command.! must runSuccessfully
  }

  override def sshCommand(command: String, executedBy: User = owner) {
    val hostname = masterHostname(executedBy).getOrElse(fail("No master to ssh to"))
    val sshLine = s"""ssh $sshFlags ${executedBy.handle}@$hostname $command"""
    info(s"Executing command in cluster master: $sshLine")
    sshLine.! must runSuccessfully
  }

  private def masterHostname(executedBy: User = owner): Option[String] = (for {
    JObject(children) <- describe(executedBy)
    JField("master", JObject(masterFields)) <- children
    JField("hostname", JString(hostname)) <- masterFields
  } yield hostname).headOption
}

object CommandLineManagedCluster extends Cluster.Factory with MustMatchers {

  /** Creates the cluster and returns its ID in case of success. */
  def apply(
      clusterSize: Int,
      owner: User,
      services: Seq[String] = Seq.empty,
      shared: Boolean = false)(implicit info: Informer): CommandLineManagedCluster = {
    val nameFlag = "--name default-services"
    val sizeFlag = s"--size $clusterSize"
    val srvFlag = servicesFlag(services, owner)
    val sharedFlag = if (shared) "--shared" else ""
    val command = s"cosmos -c ${owner.cosmosrcPath} create $sharedFlag $nameFlag $sizeFlag $srvFlag"
    info(s"Calling create cluster with command '$command'")
    val commandOutput = command.lines_!.toList
    commandOutput.foreach(info(_))
    val expectedPrefix = "Provisioning new cluster "
    val id = commandOutput
      .find(_.startsWith(expectedPrefix))
      .getOrElse(fail(s"unexpected create command output: ${commandOutput.mkString("\n")}"))
      .substring(expectedPrefix.length)
    info(s"Cluster created with id $id")
    new CommandLineManagedCluster(id, owner, shared)(info)
  }

  private def servicesFlag(services: Seq[String], user: User) = {
    val availableServices = listServices(user)
    val optionalServices = services.filter(availableServices.contains)
    if (optionalServices.nonEmpty) s"--services ${optionalServices.mkString(" ")}" else ""
  }

  private def listServices(executedBy: User): Seq[String] = {
    val command = s"cosmos -c ${executedBy.cosmosrcPath} list-services"
    val commandOutput = stringToProcess(command).lines.toSeq
    val optionalServices = commandOutput.tail.map(_.trim)
    optionalServices
  }
}
