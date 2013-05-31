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
  def apply(host: String, port: Int, username: String, password: String): MySqlConnDetails =
    MySqlConnDetails(host, port, username, password, dbName = "")
}

/**
 * Provider of database connection configuration.
 */
trait MySqlConnDetailsComponent {
  def connectionDetails: MySqlConnDetails
}