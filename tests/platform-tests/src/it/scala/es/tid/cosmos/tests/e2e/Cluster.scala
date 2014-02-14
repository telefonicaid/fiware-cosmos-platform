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

package es.tid.cosmos.tests.e2e

import scala.language.postfixOps
import scala.sys.process._

import net.liftweb.json._
import org.scalatest.Informer
import org.scalatest.matchers.MustMatchers
import org.scalatest.verb.MustVerb

class Cluster(id: String, user: User)(implicit info: Informer) extends MustVerb with MustMatchers with Patience {
  private implicit val Formats = net.liftweb.json.DefaultFormats

  def asUser(otherUser: User) = new Cluster(id, otherUser)

  def isListed: Boolean = (s"cosmos -c ${user.cosmosrcPath} list" lines_!).exists(_.contains(id))

  def describe = parse(s"cosmos -c ${user.cosmosrcPath} show $id" !! ProcessLogger(info(_)))

  def state: Option[String] = (describe \ "state").extractOpt[String]

  def users: Seq[String] = (describe \ "users").toOpt match {
    case Some(users) =>
      for {
        JObject(fields) <- users
        JField("username", JString(handle)) <- fields
      } yield handle
    case None => Seq.empty
  }

  def addUser(clusterUser: String): Int =
    s"cosmos -c ${user.cosmosrcPath} adduser $id $clusterUser" ! ProcessLogger(info(_))

  def removeUser(clusterUser: String): Int =
    s"cosmos -c ${user.cosmosrcPath} rmuser $id $clusterUser" ! ProcessLogger(info(_))

  def ensureState(expectedState: String) {
    eventually {
      assert(
        state == Some(expectedState),
        s"ExpectedState [${Some(expectedState)}] not reached. Actual: [$state]." +
          s"Cluster info:\n${pretty(render(describe))}"
      )
    }
  }

  def terminate() {
    info(s"Calling terminate on cluster $id")
    s"cosmos -c ${user.cosmosrcPath} terminate $id" ! ProcessLogger(info(_))
  }
}

object Cluster {
  def create(size: Int, user: User, services: Seq[String] = Seq())(implicit info: Informer): Cluster = {
    val ExpectedPrefix = "Provisioning new cluster "
    val flatServices = services.mkString(" ")
    val servicesCommand = if (services.nonEmpty) s"--services $flatServices" else ""
    info(s"Calling create cluster")
    val commandOutput = s"cosmos -c ${user.cosmosrcPath} create --name default-services --size $size $servicesCommand"
      .lines_!.toList
    commandOutput.foreach(info(_))
    val id = commandOutput
      .filter(_.startsWith(ExpectedPrefix))
      .head.substring(ExpectedPrefix.length)
    info(s"Cluster created with id $id")
    new Cluster(id, user)
  }
}
