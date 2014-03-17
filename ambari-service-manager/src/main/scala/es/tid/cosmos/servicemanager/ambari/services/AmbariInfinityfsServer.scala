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

package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.services.{Service, InfinityfsServer}

object AmbariInfinityfsServer extends AmbariService with NoConfiguration {
  override val service: Service = InfinityfsServer
  override val components: Seq[ComponentDescription] = Seq(
    ComponentDescription.masterComponent("INFINITY_HFS_MASTER_SERVER"),
    ComponentDescription.slaveComponent("INFINITY_HFS_SLAVE_PROXY")
  )
}
