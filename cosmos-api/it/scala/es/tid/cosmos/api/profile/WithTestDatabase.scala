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

package es.tid.cosmos.api.profile

import java.util.Properties
import java.io.FileReader

import anorm._
import play.api.Play.current
import play.api.db.DB
import play.api.test.FakeApplication

import es.tid.cosmos.api.mocks.{TestWithDbGlobal, WithTestApplication}
import play.api.db.evolutions.EvolutionsPlugin

private[profile] class WithTestDatabase(additionalConfiguration: Map[String, String] = Map.empty)
  extends WithTestApplication(
    additionalConfiguration = additionalConfiguration ++ WithTestDatabase.loadDatabaseProperties(),
    playGlobal = new TestWithDbGlobal) {

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

  val TestPropertiesFile = "cosmos-api/it/resources/test.properties"

  def loadDatabaseProperties(): Map[String, String] = {
    val props = new Properties()
    props.load(new FileReader(TestPropertiesFile))
    import scala.collection.JavaConverters._
    val propMap = props.asScala.toMap
    propMap.updated("db.default.url", s"jdbc:mysql://localhost/${propMap("db.default.database")}")
  }
}
