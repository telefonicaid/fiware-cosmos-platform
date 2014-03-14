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

package es.tid.cosmos.servicemanager.ambari

import scala.concurrent.{Await, Future}
import scala.concurrent.Future.successful
import scala.concurrent.duration._
import scala.language.postfixOps

import org.mockito.BDDMockito.given
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager.ambari.rest.{Host, Cluster}
import es.tid.cosmos.servicemanager.ambari.services.AmbariHdfs

class ServiceMasterExtractorTest extends FlatSpec with MustMatchers with MockitoSugar {

  "An Extractor" must "extract host containing service master components" in {
    val cluster = mock[Cluster]
    val hosts @ List(host1, host2, host3) = List(mock[Host], mock[Host], mock[Host])
    given(cluster.getHosts).willReturn(successful(hosts))
    given(host1.getComponentNames).willReturn(List("other"))
    given(host2.getComponentNames).willReturn(List("DATANODE"))
    given(host3.getComponentNames).willReturn(List("NAMENODE", "HDFS_CLIENT"))
    val master: Future[Host] = ServiceMasterExtractor.getServiceMaster(cluster, AmbariHdfs)
    val host = Await.result(master, 1 second)
    host must equal(host3)
  }
}
