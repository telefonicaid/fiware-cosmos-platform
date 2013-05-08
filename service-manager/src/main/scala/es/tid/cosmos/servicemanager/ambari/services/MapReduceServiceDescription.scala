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
 * Representation of the Map Reduce service.
 */
object MapReduceServiceDescription extends AmbariServiceDescription {

  val name: String = "MAPREDUCE"

  val components: Seq[ComponentDescription] = Seq(
    new ComponentDescription("JOBTRACKER", isMaster = true),
    new ComponentDescription("TASKTRACKER", isMaster = false),
    new ComponentDescription("MAPREDUCE_CLIENT", isMaster = true))
}
