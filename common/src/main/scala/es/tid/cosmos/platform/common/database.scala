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

import com.typesafe.config._
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
 * All the information needed to create a database connection.
 */
case class MySqlConnDetails(
    host: String,
    port: Int,
    username: String,
    password: String,
    dbName: String)

object MySqlConnDetails {
  private val DefaultPort: Int = 3306

  def apply(host: String, port: Int, username: String, password: String): MySqlConnDetails =
    MySqlConnDetails(host, port, username, password, dbName = "")

  def fromConfig(c: Config) =
    MySqlConnDetails(
      host = c.getString("ial.db.host"),
      port = c.withFallback(ConfigValueFactory.fromAnyRef(DefaultPort, "ial.db.port"))
        .getInt("ial.db.port"),
      username = c.getString("ial.db.username"),
      password = c.getString("ial.db.password"),
      dbName = c.getString("ial.db.name")
    )
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
