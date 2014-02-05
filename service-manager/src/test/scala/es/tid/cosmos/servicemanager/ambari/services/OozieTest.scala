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

class OozieTest extends FlatSpec with MustMatchers  {

  "An Oozie service" must "have an oozie server and a client" in {
    val description = Oozie
    description.name must equal("OOZIE")
    description.components must (
      have length 2 and
        contain(ComponentDescription.masterComponent("OOZIE_SERVER")) and
        contain(ComponentDescription.masterComponent("OOZIE_CLIENT").makeClient))
  }
}
