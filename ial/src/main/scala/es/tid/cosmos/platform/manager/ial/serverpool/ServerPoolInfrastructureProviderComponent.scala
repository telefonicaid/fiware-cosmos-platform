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

package es.tid.cosmos.platform.manager.ial.serverpool

import es.tid.cosmos.platform.common.MySqlDatabase
import es.tid.cosmos.platform.manager.ial.InfrastructureProviderComponent

/**
 * A trait satisfying infrastructure provider component supported by a server pool based provider.
 *
 * @author sortega
 */
trait ServerPoolInfrastructureProviderComponent extends InfrastructureProviderComponent {

  override lazy val infrastructureProvider = {
    val dbHost = "localhost"
    val dbPort = 3306
    val dbUser = "cosmos"
    val dbPassword = "cosmos"
    val dbName = "cosmos"
    val db = new MySqlDatabase(dbHost, dbPort, dbUser, dbPassword, dbName)
    val dao = new SqlServerPoolDao(db)
    new ServerPoolInfrastructureProvider(dao)
  }
}
