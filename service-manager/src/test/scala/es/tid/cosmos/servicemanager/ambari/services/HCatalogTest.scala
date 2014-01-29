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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.ComponentDescription

class HCatalogTest extends FlatSpec with MustMatchers {

  "An HCatalog service" must "have an HClient client" in {
    val description = HCatalog
    description.name must equal("HCATALOG")
    description.components must (
      have length 1 and contain(ComponentDescription("HCAT", isMaster = true, isClient = true))
    )
  }
}
