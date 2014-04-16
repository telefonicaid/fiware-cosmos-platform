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

package es.tid.cosmos.common

import java.sql.{DriverManager, Connection}
import scala.util.Try

import com.typesafe.config._
import org.squeryl.{Session, SessionFactory}
import org.squeryl.adapters.MySQLInnoDBAdapter
import org.squeryl.internals.DatabaseAdapter

/**
 * A trait of an SQL database.
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
    dbName: String) {
  val asJdbc = s"jdbc:mysql://$host:$port/$dbName"
}

object MySqlConnDetails {
  private val DefaultPort: Int = 3306

  def apply(host: String, port: Int, username: String, password: String): MySqlConnDetails =
    MySqlConnDetails(host, port, username, password, dbName = "")

  def fromConfig(c: Config) =
    MySqlConnDetails(
      host = c.getString("db.default.host"),
      port = c.withFallback(ConfigValueFactory.fromAnyRef(DefaultPort, "db.default.port"))
        .getInt("db.default.port"),
      username = c.getString("db.default.user"),
      password = c.getString("db.default.pass"),
      dbName = c.getString("db.default.name")
    )
}

/**
 * A trait of a MySQL database.
 */
class MySqlDatabase(c: MySqlConnDetails) extends SqlDatabase {

  /* Initialize MySQL JDBC driver, which registers the connection chain prefix on DriverManager. */
  Class.forName("com.mysql.jdbc.Driver")

  def connect: Try[Connection] = Try(DriverManager.getConnection(c.asJdbc, c.username, c.password))

  override val adapter = new MySQLInnoDBAdapter
}
