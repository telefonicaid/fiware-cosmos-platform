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
import es.tid.cosmos.servicemanager.ambari.configuration.{ConfigurationBundle, ConfigurationKeys}
import es.tid.cosmos.servicemanager.{ServiceDescription, ComponentDescription}

class ServiceWithConfigurationFileTest extends FlatSpec with MustMatchers {
  import ServiceWithConfigurationFile.decorateWithFileConfiguration

  val dummyAmbariService = new AmbariServiceDescription {
    override lazy val name: String = ???
    override lazy val components: Seq[ComponentDescription] = ???
    override def contributions(
        properties: Map[ConfigurationKeys.Value, String]): ConfigurationBundle = ???
  }
  val dummyConfigDir = "/tmp/null"

  val acmeService = new ServiceDescription{
    override val name: String = "Acme-Service"
    override val components: Seq[ComponentDescription] = Seq.empty
  }
  val acmeConfigPath = this.getClass.getResource("").getPath

  "A configuration file service decorator" must "ignore any AmbariServiceDescription services" in {
    decorateWithFileConfiguration(dummyAmbariService)(dummyConfigDir) must be (dummyAmbariService)
  }

  it must "decorate service descriptions with file configuration" in {
    val decorated = decorateWithFileConfiguration(acmeService)(acmeConfigPath)
    val contributions = decorated.contributions(properties = Map.empty)
    decorated must not be acmeService
    contributions.global must be ('defined)
    contributions.core must be ('defined)
    contributions.services must have length 1

    contributions.global.get.properties("global.example") must be ("acmeGlobal")
    contributions.core.get.properties("core.example") must be ("acmeCore")
    contributions.services.head.properties("service.example") must be ("acmeService")
  }
}

