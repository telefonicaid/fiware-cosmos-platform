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

package es.tid.cosmos.servicemanager.configuration

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class ConfigurationBundleTest extends FlatSpec with MustMatchers {

  "A Bundle" must "return no configurations where it has none" in {
    ConfigurationBundle(None, None, List()).configurations must be ('empty)
  }

  it must "return only the optional configurations that have a value" in {
    val global = GlobalConfiguration(Map.empty)
    val core = CoreConfiguration(Map.empty)
    val services = List(ServiceConfiguration("service", Map.empty))
    ConfigurationBundle(Some(global), None, services).configurations must equal (global :: services)
    ConfigurationBundle(None, Some(core), services).configurations must equal (core :: services)
    ConfigurationBundle(
      Some(global), Some(core), services).configurations must equal (global :: core :: services)
    ConfigurationBundle(None, None, services).configurations must equal (services)
  }
}
