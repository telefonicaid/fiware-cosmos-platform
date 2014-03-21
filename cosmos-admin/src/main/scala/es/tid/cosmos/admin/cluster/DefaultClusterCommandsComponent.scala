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

package es.tid.cosmos.admin.cluster

import es.tid.cosmos.servicemanager.ServiceManagerComponent

trait DefaultClusterCommandsComponent extends ClusterCommandsComponent {
  this: ServiceManagerComponent =>

  override lazy val clusterCommands: ClusterCommands = new DefaultClusterCommands(serviceManager)
}
