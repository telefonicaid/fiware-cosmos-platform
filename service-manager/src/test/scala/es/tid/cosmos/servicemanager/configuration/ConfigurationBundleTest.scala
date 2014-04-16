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

import es.tid.cosmos.servicemanager.services.Hdfs

class ConfigurationBundleTest extends FlatSpec with MustMatchers {

  "A Bundle" must "return no configurations where it has none" in {
    ConfigurationBundle(None, None, List()).configurations must be ('empty)
  }

  it must "return only the optional configurations that have a value" in {
    val global = GlobalConfiguration(Map.empty)
    val core = CoreConfiguration(Map.empty)
    val services = List(ServiceConfiguration("service", Map.empty, Hdfs))
    ConfigurationBundle(Some(global), None, services).configurations must equal (global :: services)
    ConfigurationBundle(None, Some(core), services).configurations must equal (core :: services)
    ConfigurationBundle(
      Some(global), Some(core), services).configurations must equal (global :: core :: services)
    ConfigurationBundle(None, None, services).configurations must equal (services)
  }
}
