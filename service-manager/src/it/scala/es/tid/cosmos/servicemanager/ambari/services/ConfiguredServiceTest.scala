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

import es.tid.cosmos.common.scalatest.resources.TestResourcePaths
import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDescriptionFactory._
import es.tid.cosmos.servicemanager.services.Service

trait ConfiguredServiceTest extends FlatSpec with MustMatchers with TestResourcePaths {

  val dynamicProperties: Map[ConfigurationKeys.Value, String]
  val service: Service

  def contributions = toAmbariService(service, resourcesConfigDirectory)
    .contributions(dynamicProperties)
}
