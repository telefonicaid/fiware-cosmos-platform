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

package es.tid.cosmos.tests.e2e

import scala.concurrent.duration._
import scala.language.postfixOps
import scala.util.Try

import com.typesafe.config.{ConfigFactory, Config}
import org.scalatest.{Tag, BeforeAndAfterAll, FeatureSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.common.scalatest.tags.{TaggedTests, EndToEndTest}
import es.tid.cosmos.tests.e2e.cluster.Cluster

abstract class E2ETestBase extends FeatureSpec with MustMatchers with Patience
  with FutureMatchers with BeforeAndAfterAll with TaggedTests {
  implicit val testConfig: Config = {
    val configFile = Option(System.getProperty("testConfig")).getOrElse("test.conf")
    ConfigFactory.load(getClass.getClassLoader, configFile)
  }
  val restTimeout = testConfig.getInt("restTimeout").seconds

  override val testsTag: Tag = EndToEndTest

  private var usersToDelete = List.empty[LazyVal[User]]
  private var clustersToDelete = List.empty[LazyVal[Cluster]]

  def withNewUser(body: LazyVal[User] => Unit) {
    withNewUsers(1)(users => body(users.head))
  }

  def withNewUsers(numberOfUsers: Int)(body: Seq[LazyVal[User]] => Unit) {
    val users = (1 to numberOfUsers).map(_ => new LazyVal(new User()))
    val executionResult = Try(body(users))
    usersToDelete ++= users
    executionResult.get
  }

  def withNewCluster(
      size: Int,
      owner: LazyVal[User],
      services: Seq[String] = Seq(),
      shared: Boolean = false)(body: LazyVal[Cluster] => Unit) {
    val cluster = new LazyVal(Cluster(size, owner, services))
    val executionResult = Try(body(cluster))
    clustersToDelete ::= cluster
    executionResult.get
  }

  override def afterAll() {
    clustersToDelete.foreach(_.close())
    usersToDelete.foreach(_.close())
  }

  def resource(resourcePath: String): String = getClass.getResource(resourcePath).getFile
}
