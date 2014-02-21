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

package es.tid.cosmos.servicemanager.production

import es.tid.cosmos.common.ConfigComponent
import es.tid.cosmos.platform.ial.libvirt.LibVirtInfrastructureProviderComponent
import es.tid.cosmos.servicemanager.ambari.AmbariServiceManagerComponent
import es.tid.cosmos.servicemanager.clusters.sql.SqlClusterDaoComponent

trait ProductionServiceManagerComponent
    extends AmbariServiceManagerComponent
    with LibVirtInfrastructureProviderComponent with SqlClusterDaoComponent {
  this: ConfigComponent =>
}
