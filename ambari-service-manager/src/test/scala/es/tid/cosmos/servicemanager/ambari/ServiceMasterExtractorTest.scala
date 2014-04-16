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
