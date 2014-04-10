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
