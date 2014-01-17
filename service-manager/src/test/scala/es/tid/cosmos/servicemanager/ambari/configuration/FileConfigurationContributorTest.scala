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

package es.tid.cosmos.servicemanager.ambari.configuration

import com.typesafe.config.{Config, ConfigFactory}
import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers

class FileConfigurationContributorTest extends FlatSpec with MustMatchers with OneInstancePerTest {
  val masterName = "myMasterNode"
  val maxMapTasks = "10"
  val maxReduceTasks = "5"
  val expectedGlobal = GlobalConfiguration(Map(
    "global.example.string" -> s"global-$masterName$maxMapTasks$maxReduceTasks",
    "global.example.boolean" -> true,
    "global.example.number" -> 29
  ))
  val expectedCore = CoreConfiguration(Map(
    "core.example" -> s"core-$masterName$maxMapTasks$maxReduceTasks"))
  val expectedService = ServiceConfiguration("test-service-site",
    Map("service.example" -> s"service-$masterName$maxMapTasks$maxReduceTasks")
  )
  val expectedServiceList = (1 to 2).toList.map(index =>
    ServiceConfiguration(s"test-service-site$index",
        Map(s"service.example$index" -> s"service$index-$masterName$maxMapTasks$maxReduceTasks"))
  )

  val full = new FileConfigurationContributor {
    override protected val configName: String = "global-core-service"
  }
  val noGlobal = new FileConfigurationContributor {
    override protected val configName: String = "core-service"
  }
  val noCore = new FileConfigurationContributor {
    override protected val configName: String = "global-service"
  }
  val justService = new FileConfigurationContributor {
    override protected val configName: String = "service"
  }
  val noService = new FileConfigurationContributor {
    override protected val configName: String = "no-service"
  }

  val serviceList = new FileConfigurationContributor {
    override protected val configName: String = "service-list"
  }

  val properties = Map(
    ConfigurationKeys.MasterNode -> masterName,
    ConfigurationKeys.MaxMapTasks -> maxMapTasks,
    ConfigurationKeys.MaxReduceTasks -> maxReduceTasks
  )

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
}
