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

import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys.MasterNode

class OozieIT extends ConfiguredServiceTest  {

  override val dynamicProperties = Map(
    MasterNode -> "aMasterNodeName"
  )

  override def configurator = AmbariOozie.configurator(None, resourcesConfigDirectory)

  "An Oozie service" must "have global and service configuration contributions without core" in {
    contributions.global must be ('defined)
    contributions.core must not be 'defined
    contributions.services must have length 1
  }
}
