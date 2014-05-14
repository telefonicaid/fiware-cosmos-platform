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

package es.tid.cosmos.servicemanager.configuration

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class ConfigurationBundleTest extends FlatSpec with MustMatchers {

  "A configuration bundle" must "return no configurations where it has none" in {
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

  it must "be merged with other bundle" in {
    val global = GlobalConfiguration(Map("a" -> 1))
    val core1 = CoreConfiguration(Map("b" -> 2))
    val core2 = CoreConfiguration(Map("c" -> 3))
    val core12 = CoreConfiguration(Map("b" -> 2, "c" -> 3))
    val service1 = ServiceConfiguration("service1", Map("d" -> 4))
    val service2a = ServiceConfiguration("service2", Map("e" -> 5))
    val service2b = ServiceConfiguration("service2", Map("f" -> 6))
    val service2 = ServiceConfiguration("service2", Map("e" -> 5, "f" -> 6))

    val left = ConfigurationBundle(Some(global), Some(core1), List(service1, service2a))
    val right = ConfigurationBundle(None, Some(core2), List(service2b))

    val merged = left.merge(right)
    merged.global must be (Some(global))
    merged.core must be (Some(core12))
    merged.services must have size 2
    merged.services must contain (service1)
    merged.services must contain (service2)
  }

  it must "throw ConfigurationConflict when merged with a conflicting system configuration" in {
    evaluating {
      ConfigurationBundle(GlobalConfiguration(Map("a" -> 1)))
        .merge(ConfigurationBundle(GlobalConfiguration(Map("a" -> 2))))
    } must produce [ConfigurationConflict]
  }

  it must "throw ConfigurationConflict when merged with a conflicting service configuration" in {
    evaluating {
      ConfigurationBundle(ServiceConfiguration("service", Map("a" -> 1)))
        .merge(ConfigurationBundle(ServiceConfiguration("service", Map("a" -> 2))))
    } must produce [ConfigurationConflict]
  }
}
