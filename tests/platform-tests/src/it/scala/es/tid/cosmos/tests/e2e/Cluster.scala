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

import net.liftweb.json.parse
import org.scalatest.FlatSpec
import org.scalatest.concurrent.{Eventually, IntegrationPatience}
import org.scalatest.matchers.MustMatchers
import org.scalatest.time.{Seconds, Minutes, Span}
import org.scalatest.verb.MustVerb

class Cluster(id: String) extends MustVerb with MustMatchers with Eventually with IntegrationPatience {
  private implicit val Formats = net.liftweb.json.DefaultFormats
  implicit override val patienceConfig = PatienceConfig(
    timeout = scaled(Span(30, Minutes)),
    interval = scaled(Span(10, Seconds)))

  def isListed: Boolean = ("cosmos list" lines_!).exists(_.contains(id))

  def describe = parse(s"cosmos show $id" !! ProcessLogger(println(_)))

  def state: Option[String] = (describe \ "state").extractOpt[String]

  def ensureState(expectedState: String) {
    eventually (state must be === Some(expectedState))
  }

  def terminate() {
    s"cosmos terminate $id" !! ProcessLogger(println(_))
  }
}

object Cluster extends FlatSpec with MustMatchers {
  def create(size: Int, services: Seq[String] = Seq()): Cluster = {
    val ExpectedPrefix = "Provisioning new cluster "
    val flatServices = services.mkString(" ")
    val servicesCommand = if (services.nonEmpty) s"--services $flatServices" else ""
    val id = s"cosmos create --name default-services --size $size $servicesCommand"
      .lines_!(ProcessLogger(println(_)))
      .filter(_.startsWith(ExpectedPrefix))
      .head.substring(ExpectedPrefix.length)
    println(s"Cluster created with id $id")
    new Cluster(id)
  }
}
