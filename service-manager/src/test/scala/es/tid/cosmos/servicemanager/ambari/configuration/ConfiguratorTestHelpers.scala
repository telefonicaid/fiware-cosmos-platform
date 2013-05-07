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

import es.tid.cosmos.servicemanager.ambari._
import scala.Some

object ConfiguratorTestHelpers {

  def properties(confType: String, number: Int) =
    Map(s"some${confType}Content$number" -> s"somevalue$number")

  def propertiesUpTo(confType: String, number: Int) =
    (1 to number).map(properties(confType, _)).reduce(_++_)

  def contributionsWithNumber(number: Int) = {
    ConfigurationBundle(
      Some(GlobalConfiguration("version1", properties("Global", number))),
      Some(CoreConfiguration("version1", properties("Core", number))),
      List(ServiceConfiguration(s"service-site$number", "version1", properties("Service", number))))
  }

  def mergedCoreConfiguration(number: Int) =
    new CoreConfiguration("version1", propertiesUpTo("Core", number))

  def mergedGlobalConfiguration(number: Int, instance: AmbariServiceManager, masterName: String) =
    GlobalConfiguration("version1",
      instance.contributions(masterName).global.get.properties ++ propertiesUpTo("Global", number))

  def expectedConfigurationOf(number: Int) = properties("Service", number)
}
