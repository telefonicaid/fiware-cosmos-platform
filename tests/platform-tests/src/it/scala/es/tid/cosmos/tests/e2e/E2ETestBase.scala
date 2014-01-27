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

import es.tid.cosmos.platform.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.platform.common.scalatest.tags.EndToEndTest

abstract class E2ETestBase extends FeatureSpec with MustMatchers with Patience
  with FutureMatchers with BeforeAndAfterAll {
  implicit val testConfig: Config = ConfigFactory.load(getClass.getClassLoader, "test.conf")
  val restTimeout = testConfig.getInt("restTimeout").seconds

  override def tags: Map[String, Set[String]] = {
    val originalTags = super.tags
    (for {
    test <- testNames
    } yield (test, originalTags.getOrElse(test, Set()) + EndToEndTest.name)).toMap
  }

  private var usersToDelete = List.empty[User]

  def withNewUser(body: User => Unit) {
    val user = new User()
    val executionResult = Try(body(user))
    usersToDelete = user :: usersToDelete
    executionResult.get
  }

  override def afterAll() {
    usersToDelete.foreach(_.delete())
  }
}
