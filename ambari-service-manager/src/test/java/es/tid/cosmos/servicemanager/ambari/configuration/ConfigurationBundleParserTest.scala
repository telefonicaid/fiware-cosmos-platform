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

import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.configuration.ConfigurationBundle

class ConfigurationBundleParserTest extends FlatSpec with MustMatchers {

  val parser = new ConfigurationBundleParser(true)
  val parserWithoutService = new ConfigurationBundleParser(false)

  "A configuration parser" must "parse an empty configuration" in {
    val conf = ConfigFactory.empty()
    parser.parse(conf) must be (ConfigurationBundle.NoConfiguration)
  }

  it must "reject the services section if no service is associated" in {
    val configWithServices = ConfigFactory.parseString(""" "services" = [] """)
    parser.parse(configWithServices) must not be null
    evaluating {
      parserWithoutService.parse(configWithServices)
    } must produce [IllegalArgumentException]
  }

  it must "parse all service configurations" in {
    val configWithServices = ConfigFactory.parseString(
      """
        |"services" = [{
        |  "configType" = "foo"
        |  "properties" {
        |    "foo1" = "val1"
        |  }
        |}, {
        |  "configType" = "bar"
        |  "properties" {
        |    "bar1" = "val1"
        |    "bar2" = 42
        |  }
        |}]
      """.stripMargin)
    val services = parser.parse(configWithServices).services
    services.map(_.configType).toSet must be (Set("foo", "bar"))
    services.find(_.configType == "foo").get.properties must be (Map(
      "foo1" -> "val1"
    ))
    services.find(_.configType == "bar").get.properties must be (Map(
      "bar1" -> "val1",
      "bar2" -> 42
    ))
  }

  it must "parse a global configuration" in {
    val globalConfig = ConfigFactory.parseString(
      """
        |"global" = {
        |  "properties" {
        |    "foo1" = "val1"
        |  }
        |}
      """.stripMargin)
    val global = parser.parse(globalConfig).global.get
    global.properties must be (Map(
      "foo1" -> "val1"
    ))
  }

  it must "parse a core configuration" in {
    val coreConfig = ConfigFactory.parseString(
      """
        |"core" = {
        |  "properties" {
        |    "foo1" = "val1"
        |  }
        |}
      """.stripMargin)
    val core = parser.parse(coreConfig).core.get
    core.properties must be (Map(
      "foo1" -> "val1"
    ))
  }
}
