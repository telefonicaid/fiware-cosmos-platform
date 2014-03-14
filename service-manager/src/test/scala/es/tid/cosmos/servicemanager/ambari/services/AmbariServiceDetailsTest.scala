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
import es.tid.cosmos.servicemanager.services.Service

class AmbariServiceDetailsTest extends FlatSpec with MustMatchers {

  class AmbariTestService(
      serverServices: Seq[String] = Seq.empty,
      clientServices: Seq[String] = Seq.empty) extends AmbariServiceDetails{

    override val service: Service = null

    override val components: Seq[ComponentDescription] =
      serverServices.map(ComponentDescription.masterComponent) ++
      clientServices.map(ComponentDescription.masterComponent).map(_.makeClient)
  }

  it must "be considered running for state 'INSTALL' when its components are of client type" in {
    val description = new AmbariTestService(clientServices = Seq("component1", "component2"))
    description.runningState must be (InstalledService)
  }

  it must "be considered running for state 'STARTED' in any other case" in {
    val description = new AmbariTestService(
      serverServices = Seq("component1"), clientServices = Seq("component2")
    )
    description.runningState must be (StartedService)
  }
}
