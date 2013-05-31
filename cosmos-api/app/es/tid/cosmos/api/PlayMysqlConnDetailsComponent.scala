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

package es.tid.cosmos.api

import play.Play
import es.tid.cosmos.platform.common.{MySqlConnDetails, MySqlConnDetailsComponent}

/**
 * Component that pulls connection details from Play! configuration.
 */
trait PlayMySqlConnDetailsComponent extends MySqlConnDetailsComponent {

  def connectionDetails: MySqlConnDetails = {
    val conf = Play.application().configuration()
    val username = conf.getString("db.default.user")
    val password = conf.getString("db.default.pass")
    val List(host, strPort, dbName): List[String] =
      PlayMySqlConnDetailsComponent.JdbcPattern.unapplySeq(conf.getString("db.default.url")).get
    val port =
      if (strPort != null) strPort.toInt
      else PlayMySqlConnDetailsComponent.DefaultPort
    MySqlConnDetails(host, port, username, password, dbName)
  }
}

object PlayMySqlConnDetailsComponent {
  val DefaultPort: Int = 3306
  val JdbcPattern = "jdbc:mysql://([^:/]+)(?::(\\d+))?/(.*)".r
}
