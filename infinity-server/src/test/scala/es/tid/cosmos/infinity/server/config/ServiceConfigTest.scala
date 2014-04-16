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

package es.tid.cosmos.infinity.server.config

import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class ServiceConfigTest extends FlatSpec with MustMatchers {

  val sampleConfig = ConfigFactory.parseString(
    """infinity.server {
      |  services {
      |    namenode {
      |      infinity {
      |        hostname = "localhost"
      |        port = 8009
      |      }
      |    }
      |
      |    datanode1 {
      |      infinity {
      |        hostname = "localhost"
      |        port = 8011
      |      }
      |    }
      |
      |    active = namenode
      |  }
      |}
      |
    """.stripMargin)

  val configWithoutActive = ConfigFactory.parseString(
    """infinity.server {
      |  services {
      |    namenode {
      |      infinity {
      |        hostname = "localhost"
      |        port = 8009
      |      }
      |    }
      |
      |    datanode1 {
      |      infinity {
      |        hostname = "localhost"
      |        port = 8011
      |      }
      |    }
      |  }
      |}
      |
    """.stripMargin)

  "Service config" must "retrieve the config for namenode" in {
    ServiceConfig("namenode", sampleConfig) must
      be (Some(ServiceConfig("namenode", "localhost", 8009)))
  }

  it must "fail to retrieve the config for unknown service" in {
    ServiceConfig("unknown", sampleConfig) must be (None)
  }

  it must "retrieve the config for active service" in {
    ServiceConfig.active(sampleConfig) must be (ServiceConfig("namenode", "localhost", 8009))
  }

  it must "fail to retrieve the config for active service when missing" in {
    evaluating { ServiceConfig.active(configWithoutActive) } must produce[IllegalStateException]
  }
}
