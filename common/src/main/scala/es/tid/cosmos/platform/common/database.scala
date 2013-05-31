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

import java.sql.{DriverManager, Connection}
import scala.util.Try

import org.squeryl.{Session, SessionFactory}
import org.squeryl.internals.DatabaseAdapter
import org.squeryl.adapters.MySQLAdapter

/**
 * A trait of an SQL databse.
 */
trait SqlDatabase extends SessionFactory {

  /**
   * Obtain the Squeryl database adapter corresponding to this SQL database.
   */
  val adapter: DatabaseAdapter

  /**
   * Create a new connection to the database.
   */
  def connect: Try[Connection]

  /**
   * Create a new Squeryl session from a connection to the database.
   */
  def newSession: Session = Session.create(connect.get, adapter)
}

/**
 * A trait of a MySQL database.
 */
class MySqlDatabase(c: MySqlConnDetails) extends SqlDatabase {

  /* Initialize MySQL JDBC driver, which registers the connection chain prefix on DriverManager. */
  Class.forName("com.mysql.jdbc.Driver")

  def connect: Try[Connection] = Try(DriverManager.getConnection(
    s"jdbc:mysql://${c.host}:${c.port}/${c.dbName}", c.username, c.password))

  override val adapter = new MySQLAdapter
}
