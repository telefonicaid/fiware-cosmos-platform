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

import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.{ServiceDescription, ComponentDescription}
import es.tid.cosmos.common.scalatest.resources.TestResourcePaths

class AmbariServiceDescriptionFactoryTest extends FlatSpec with MustMatchers
    with TestResourcePaths {

  import AmbariServiceDescriptionFactory.toAmbariService

  val dummyAmbariService = new AmbariServiceDescription {
    override lazy val name: String = ???
    override lazy val components: Seq[ComponentDescription] = ???
    override def contributions(
        properties: Map[ConfigurationKeys.Value, String]): ConfigurationBundle = ???
  }
  val dummyConfigDir = "/tmp/null"

  val simpleService = new ServiceDescription{
    override val name: String = "Acme-Service"
    override val components: Seq[ComponentDescription] = Seq.empty
  }
  val acmeConfigPath = packageResourcesConfigDirectory

  val serviceWithConfig = new ServiceDescription with ConfigurationContributor {
    override val name: String = "Service-With-Configuration"
    override val components: Seq[ComponentDescription] = Seq.empty
    override def contributions(properties: ConfigProperties): ConfigurationBundle =
      ConfigurationBundle(
        GlobalConfiguration(Map("globalKey" -> "globalValue")),
        CoreConfiguration(Map("coreKey" -> "coreValue")),
        ServiceConfiguration("serviceType", Map("serviceKey" -> "serviceValue"))
      )
  }

  "A configuration file service decorator" must "ignore any AmbariServiceDescription services" in {
    toAmbariService(dummyAmbariService, dummyConfigDir) must be (dummyAmbariService)
  }

  it must "wrap simple service descriptions with file configuration" in {
    val ambariService = toAmbariService(simpleService, acmeConfigPath)
    val contributions = ambariService.contributions(properties = Map.empty)
    ambariService must not be simpleService
    contributions.global must be ('defined)
    contributions.core must be ('defined)
    contributions.services must have length 1

    contributions.global.get.properties("global.example") must be ("acmeGlobal")
    contributions.core.get.properties("core.example") must be ("acmeCore")
    contributions.services.head.properties("service.example") must be ("acmeService")
  }

  it must "wrap service descriptions that provide configuration, reusing their contribution" in {
    val ambariService = toAmbariService(serviceWithConfig, dummyConfigDir)
    val contributions = ambariService.contributions(properties = Map.empty)
    ambariService must not be serviceWithConfig
    contributions.global must be ('defined)
    contributions.core must be ('defined)
    contributions.services must have length 1

    contributions.global.get.properties("globalKey") must be ("globalValue")
    contributions.core.get.properties("coreKey") must be ("coreValue")
    contributions.services.head.properties("serviceKey") must be ("serviceValue")
  }
}

