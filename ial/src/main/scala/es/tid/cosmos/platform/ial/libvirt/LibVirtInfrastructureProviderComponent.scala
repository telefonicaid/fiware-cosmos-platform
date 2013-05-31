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

package es.tid.cosmos.platform.ial.libvirt

import es.tid.cosmos.platform.common.ConfigComponent
import es.tid.cosmos.platform.ial.InfrastructureProviderComponent
import es.tid.cosmos.platform.common.{MySqlConnDetails, MySqlDatabase}
import es.tid.cosmos.platform.ial.libvirt.jna.JnaLibVirtServer

/**
 * A trait satisfying infrastructure provider component supported by libvirt.
 */
trait LibVirtInfrastructureProviderComponent extends InfrastructureProviderComponent {
  this: ConfigComponent =>

  override val infrastructureProvider = {
    val db = new MySqlDatabase(MySqlConnDetails.fromConfig(config))
    val dao = new SqlLibVirtDao(db)
    new LibVirtInfrastructureProvider(dao,
      libvirtServerFactory = props => new JnaLibVirtServer(props),
      rootPrivateSshKey = config.getString("ial.root.ssh.private_key"))
  }
}
