/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.servicemanager.ambari.configuration

import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.resources.TestResourcePaths
import es.tid.cosmos.servicemanager.configuration.{ServiceConfiguration, CoreConfiguration, GlobalConfiguration, ConfigurationKeys}
import es.tid.cosmos.servicemanager.ambari.services.{TestService, AmbariService}
import es.tid.cosmos.servicemanager.services.Service

class FileConfigurationContributorTest extends FlatSpec with MustMatchers with OneInstancePerTest
    with TestResourcePaths {

  val masterName = "myMasterNode"
  val expectedGlobal = GlobalConfiguration(Map(
    "global.example.string" -> s"global-$masterName",
    "global.example.boolean" -> true,
    "global.example.number" -> 29
  ))
  val expectedCore = CoreConfiguration(Map(
    "core.example" -> s"core-$masterName"))
  val expectedService = ServiceConfiguration("test-service-site",
    Map("service.example" -> s"service-$masterName"), TestService
  )
  val expectedServiceList = (1 to 2).toList.map(index =>
    ServiceConfiguration(s"test-service-site$index",
      Map(s"service.example$index" -> s"service$index-$masterName"),
      TestService)
  )
  class ClasspathFileConfigurationContributor(configName: String, service: Option[Service])
    extends FileConfigurationContributor(packageResourcesConfigDirectory, configName, service)

  val full = new ClasspathFileConfigurationContributor("global-core-service", Some(TestService))
  val noGlobal = new ClasspathFileConfigurationContributor("core-service", Some(TestService))
  val noCore = new ClasspathFileConfigurationContributor("global-service", Some(TestService))
  val justService = new ClasspathFileConfigurationContributor("service", Some(TestService))
  val noService = new ClasspathFileConfigurationContributor("no-service", None)
  val serviceList = new ClasspathFileConfigurationContributor("service-list", Some(TestService))

  val properties = Map(ConfigurationKeys.MasterNode -> masterName)

  def withConfig(contributor: FileConfigurationContributor) =
    contributor.contributions(properties)

  "A builder" must "load a configuration with global, core and service" in {
    val bundle = withConfig(full)
    bundle.configurations must have length 3
    bundle.global.get must equal(expectedGlobal)
    bundle.core.get must equal(expectedCore)
    bundle.services.head must equal(expectedService)
  }

  it must "load a configuration without global" in {
    val bundle = withConfig(noGlobal)
    bundle.configurations must have length 2
    bundle.global must equal(None)
    bundle.core.get must equal(expectedCore)
    bundle.services.head must equal(expectedService)
  }

  it must "load a configuration without core" in {
    val bundle = withConfig(noCore)
    bundle.configurations must have length 2
    bundle.global.get must equal(expectedGlobal)
    bundle.core must equal(None)
    bundle.services.head must equal(expectedService)
  }

  it must "load a configuration without global and core" in {
    val bundle = withConfig(justService)
    bundle.configurations must have length 1
    bundle.global must equal(None)
    bundle.core must equal(None)
    bundle.services.head must equal(expectedService)
  }

  it must "load a configuration without service" in {
    val bundle = withConfig(noService)
    bundle.configurations must have length 2
    bundle.global.get must equal(expectedGlobal)
    bundle.core.get must equal(expectedCore)
    bundle.services must be('empty)
  }

  it must "load a configuration with a service list" in {
    val bundle = withConfig(serviceList)
    bundle.configurations must have length 2
    bundle.global must be (None)
    bundle.core must be (None)
    bundle.services must be (expectedServiceList)
  }

  it must "accept extra config properties on its constructor" in {
    val contributor = new FileConfigurationContributor(packageResourcesConfigDirectory, "extra",
      extraProperties = Map(ConfigurationKeys.MasterNode -> "extra.value"))
    val contributions = contributor.contributions(properties)
    val extraProperty = contributions.global.get.properties.get("extra.property")
    extraProperty must be (Some("extra.value"))
  }
}
