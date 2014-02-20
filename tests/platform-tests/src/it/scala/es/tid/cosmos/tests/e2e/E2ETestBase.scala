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

import scala.concurrent.duration._
import scala.language.postfixOps
import scala.util.Try

import com.typesafe.config.{ConfigFactory, Config}
import org.scalatest.{BeforeAndAfterAll, FeatureSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.common.scalatest.tags.EndToEndTest
import es.tid.cosmos.tests.e2e.cluster.Cluster

abstract class E2ETestBase extends FeatureSpec with MustMatchers with Patience
  with FutureMatchers with BeforeAndAfterAll {
  implicit val testConfig: Config = {
    val configFile = Option(System.getProperty("testConfig")).getOrElse("test.conf")
    ConfigFactory.load(getClass.getClassLoader, configFile)
  }
  val restTimeout = testConfig.getInt("restTimeout").seconds

  override def tags: Map[String, Set[String]] = {
    val originalTags = super.tags
    (for {
    test <- testNames
    } yield (test, originalTags.getOrElse(test, Set()) + EndToEndTest.name)).toMap
  }

  private var usersToDelete = List.empty[User]
  private var clustersToDelete = List.empty[Cluster]

  def withNewUser(body: User => Unit) {
    val user = new User()
    val executionResult = Try(body(user))
    usersToDelete = user :: usersToDelete
    executionResult.get
  }

  def withNewUsers(numberOfUsers: Int)(body: Seq[User] => Unit) {
    val users = (1 to numberOfUsers).map(_ => new User())
    val executionResult = Try(body(users))
    usersToDelete ++= users
    executionResult.get
  }

  def withNewCluster(size: Int, owner: User, services: Seq[String] = Seq())(body: Cluster => Unit) {
    val cluster = Cluster(size, owner, services)(info)
    val executionResult = Try(body(cluster))
    clustersToDelete ::= cluster
    executionResult.get
  }

  override def afterAll() {
    usersToDelete.foreach(_.delete())
    clustersToDelete.foreach(c => c.terminate())
  }

  def resource(resourcePath: String): String = getClass.getResource(resourcePath).getFile
}
