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

package es.tid.cosmos.infinity

import scala.concurrent.duration._

import org.apache.hadoop.conf.Configuration
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class InfinityConfigurationTest extends FlatSpec with MustMatchers {

  "An infinity configuration" must "provide a default authority" in {
    val config = configurationWithProperties(
      "fs.infinity.defaultAuthority" -> "orion-infinity.hi.inet:8080")
    config.defaultAuthority must be (Some("orion-infinity.hi.inet:8080"))
  }

  it must "provide no default authority when not configured" in {
    defaultConfiguration().defaultAuthority must be ('empty)
  }

  it must "use SSL by default" in {
    defaultConfiguration().useSsl must be (true)
  }

  it must "not use SSL when explicitly configured not to" in {
    configurationWithProperties("fs.infinity.ssl.enabled" -> false).useSsl must be (false)
  }

  it must "have a default timeout duration" in {
    defaultConfiguration().timeoutDuration must be (3.seconds)
  }

  it must "provide the configured timeout duration" in {
    val config = configurationWithProperties("fs.infinity.timeout.millis" -> 1000)
    config.timeoutDuration must be (1.second)
  }

  it must "reject non-positive timeout durations" in {
    for (invalidMillis <- Seq(-1, 0)) {
      evaluating {
        configurationWithProperties("fs.infinity.timeout.millis" -> invalidMillis).timeoutDuration
      } must produce [IllegalArgumentException]
    }
  }

  def defaultConfiguration() = configurationWithProperties()

  def configurationWithProperties(properties: (String, Any)*): InfinityConfiguration = {
    val hadoopConf = new Configuration(false)
    for ((key, value) <- properties) {
      hadoopConf.set(key, value.toString)
    }
    new InfinityConfiguration(hadoopConf)
  }
}
