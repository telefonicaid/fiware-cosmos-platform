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

package es.tid.cosmos.platform.ial.serverpool

import es.tid.cosmos.platform.common.{MySqlConnDetailsComponent, MySqlDatabase}
import es.tid.cosmos.platform.ial.InfrastructureProviderComponent

/**
 * A trait satisfying infrastructure provider component supported by a server pool based provider.
 *
 * @author sortega
 */
trait ServerPoolInfrastructureProviderComponent extends InfrastructureProviderComponent {
  this: MySqlConnDetailsComponent =>

  override lazy val infrastructureProvider = {
    val db = new MySqlDatabase(connectionDetails)
    val dao = new SqlServerPoolDao(db)
    new ServerPoolInfrastructureProvider(dao)
  }
}
