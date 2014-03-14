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

import scala.language.postfixOps
import scala.sys.process._

import net.liftweb.json._
import net.liftweb.json.JsonAST.{JString, JField, JObject}
import org.scalatest.Informer
import org.scalatest.verb.MustVerb
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.tests.e2e.{CommandLineMatchers, User}

class CommandLineManagedCluster(val id: String, val owner: User)
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
  def apply(clusterSize: Int, owner: User, services: Seq[String] = Seq.empty)
           (implicit info: Informer): CommandLineManagedCluster = {
    val nameFlag = "--name default-services"
    val sizeFlag = s"--size $clusterSize"
    val srvFlag = servicesFlag(services, owner)
    val command = s"cosmos -c ${owner.cosmosrcPath} create $nameFlag $sizeFlag $srvFlag"
    info(s"Calling create cluster with command '$command'")
    val commandOutput = command.lines_!.toList
    commandOutput.foreach(info(_))
    val expectedPrefix = "Provisioning new cluster "
    val id = commandOutput
      .find(_.startsWith(expectedPrefix))
      .getOrElse(fail(s"unexpected create command output: ${commandOutput.mkString("\n")}"))
      .substring(expectedPrefix.length)
    info(s"Cluster created with id $id")
    new CommandLineManagedCluster(id, owner)(info)
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
