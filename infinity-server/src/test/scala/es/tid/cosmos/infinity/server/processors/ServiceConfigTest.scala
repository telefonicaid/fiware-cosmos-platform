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

package es.tid.cosmos.infinity.server.processors

import akka.actor.ActorSystem
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class ServiceConfigTest extends FlatSpec with MustMatchers {

  implicit val system = ActorSystem("ServiceConfigTest")

  "Service config" must "retrieve the config for namenode" in {
    ServiceConfig("namenode") must be
      Some(ServiceConfig("namenode", "localhost", 8008, "localhost", 8009))
  }

  it must "fail to retrieve the config for unknown service" in {
    ServiceConfig("unknown") must be (None)
  }

  it must "retrieve the config for active service" in {
    ServiceConfig.active must be
      ServiceConfig("namenode", "localhost", 8008, "localhost", 8009)
  }

  it must "retrieve the config from HDFS subconfig" in {
    ServiceConfig.fromWebHdfs("localhost", 8010) must be
      Some(ServiceConfig("datanode1", "localhost", 8009, "localhost", 8010))
  }

  it must "fail to retrieve the config from unmatching HDFS subconfig" in {
    ServiceConfig.fromWebHdfs("unexisting-host", 9999) must be (None)
  }
}
