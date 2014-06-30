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

package es.tid.cosmos.api.profile

import java.util.Properties
import java.io.{File, FileReader}

import anorm._
import play.api.db.DB
import play.api.db.evolutions.EvolutionsPlugin
import play.api.test.FakeApplication

import es.tid.cosmos.api.mocks._

private[profile] class WithTestDatabase(additionalConfiguration: Map[String, String] = Map.empty)
  extends WithTestApplication(
    additionalConfiguration = additionalConfiguration ++ WithTestDatabase.loadDatabaseProperties(),
    testApp = new PlayDaoTestApplication) {

  def appWithTestDb: FakeApplication = implicitApp

  def resetDb() {
    DB.withConnection { implicit c =>
      val name = implicitApp.configuration.getString("db.default.database")
        .getOrElse(throw new IllegalStateException("No test database is defined"))
      SQL(s"drop database if exists `$name`").executeUpdate()
      SQL(s"create database `$name`").executeUpdate()
    }
    new EvolutionsPlugin(appWithTestDb).onStart()
  }
}

private[profile] object WithTestDatabase {

  private val TestPropertiesPath = "cosmos-api/it/resources/"
  private val TestProperties = new File(TestPropertiesPath, "test.properties")
  private val TestPropertiesOverrides = new File(TestPropertiesPath, "test_local.properties")

  def loadDatabaseProperties(): Map[String, String] = {
    val props = loadJavaProperties()
    import scala.collection.JavaConverters._
    val propMap = props.asScala.toMap
    propMap.updated("db.default.url",
      s"jdbc:mysql://${propMap("db.default.hostname")}/${propMap("db.default.database")}")
  }

  private def loadJavaProperties(): Properties = {
    val props = new Properties()
    props.load(new FileReader(TestProperties))
    if (TestPropertiesOverrides.exists()) {
      props.load(new FileReader(TestPropertiesOverrides))
    }
    props
  }
}
