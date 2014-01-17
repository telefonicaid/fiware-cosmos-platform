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

import es.tid.cosmos.servicemanager.ComponentDescription

/**
 * Representation of the Map Reduce 2 service.
 * The service needs to run along with YARN.
 * @see [[es.tid.cosmos.servicemanager.ambari.services.Yarn]]
 */
object MapReduce2 extends ServiceWithConfigurationFile {
  override val name: String = "MAPREDUCE2"

  override val components: Seq[ComponentDescription] = Seq(
    ComponentDescription("HISTORYSERVER", isMaster = true),
    ComponentDescription("MAPREDUCE2_CLIENT", isMaster = true, isClient = true))
}
