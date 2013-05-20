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

package es.tid.cosmos.platform.common

import scala.Some
import scala.util.{Failure, Success, Random}

import org.scalatest.{Suite, BeforeAndAfter}
import org.squeryl.Schema
import org.squeryl.PrimitiveTypeMode._

/**
 * A trait for testing against MySQL databases.
 */
trait MySqlTest extends BeforeAndAfter {
  this: Suite =>

  def dbUser: String
  def dbPassword: String
  def dbHost: String
  def dbPort: Int
  val dbName = s"test_${math.abs(Random.nextInt())}"
  lazy val db = new MySqlDatabase(dbHost, dbPort, dbUser, dbPassword, dbName)

  /**
   * The schema of the database. Override in extensions to set the Squeryl schema to use.
   */
  def schema: Schema

  /**
   * Override this function to indicate how the database is populated.
   */
  def populateInitialData {}


  private def createDatabase() { updateDatabase(s"create database $dbName") }
  private def dropDatabase() { updateDatabase(s"drop database $dbName") }

  private def updateDatabase(updateQuery: String) {
    var stmt: Option[java.sql.Statement] = None
    new MySqlDatabase(dbHost, dbPort, dbUser, dbPassword).connect match {
      case Success(c) => try {
        stmt = Some(c.createStatement())
        stmt.get.executeUpdate(updateQuery)
      } finally {
        stmt.map(_.close())
        c.close()
      }
      case Failure(e) => throw e
    }
  }

  before {
    createDatabase()
    transaction(db.newSession) {
      schema.create
      populateInitialData
    }
  }

  after {
    transaction(db.newSession) {
      schema.drop
    }
    dropDatabase()
  }
}
