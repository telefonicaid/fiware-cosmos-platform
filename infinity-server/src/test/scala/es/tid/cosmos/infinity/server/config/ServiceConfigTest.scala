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

import akka.actor.ActorSystem
import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class ServiceConfigTest extends FlatSpec with MustMatchers {

  "Service config" must "retrieve the config for namenode" in new SampleConfig {
    ServiceConfig("namenode")(system) must be (Some(
      ServiceConfig("namenode", "localhost", 8008, "localhost", 8009)))
  }

  it must "fail to retrieve the config for unknown service" in new SampleConfig {
    ServiceConfig("unknown")(system) must be (None)
  }

  it must "retrieve the config for active service" in new SampleConfig {
    ServiceConfig.active(system) must be
      ServiceConfig("namenode", "localhost", 8008, "localhost", 8009)
  }

  it must "fail to retrieve the config for active service when missing" in new SampleConfigWithoutActive {
    evaluating { ServiceConfig.active(system) } must produce[IllegalStateException]
  }

  it must "retrieve the config from HDFS subconfig" in new SampleConfig {
    ServiceConfig.fromWebHdfs("localhost", 8010)(system) must be
      Some(ServiceConfig("datanode1", "localhost", 8009, "localhost", 8010))
  }

  it must "fail to retrieve the config from unmatching HDFS subconfig" in new SampleConfig {
    ServiceConfig.fromWebHdfs("unexisting-host", 9999)(system) must be (None)
  }

  private trait SampleConfig {
    val config = ConfigFactory.parseString(
      """infinity.server {
        |  services {
        |    namenode {
        |      webhdfs {
        |        hostname = "localhost"
        |        port = 8008
        |      }
        |      infinity {
        |        hostname = "localhost"
        |        port = 8009
        |      }
        |    }
        |
        |    datanode1 {
        |      webhdfs {
        |        hostname = "localhost"
        |        port = 8010
        |      }
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
    implicit val system = ActorSystem("ServiceConfigTest", config)
  }

  private trait SampleConfigWithoutActive {
    val config = ConfigFactory.parseString(
      """infinity.server {
        |  services {
        |    namenode {
        |      webhdfs {
        |        hostname = "localhost"
        |        port = 8008
        |      }
        |      infinity {
        |        hostname = "localhost"
        |        port = 8009
        |      }
        |    }
        |
        |    datanode1 {
        |      webhdfs {
        |        hostname = "localhost"
        |        port = 8010
        |      }
        |      infinity {
        |        hostname = "localhost"
        |        port = 8011
        |      }
        |    }
        |  }
        |}
        |
      """.stripMargin)
    implicit val system = ActorSystem("ServiceConfigTest", config)
  }
}
