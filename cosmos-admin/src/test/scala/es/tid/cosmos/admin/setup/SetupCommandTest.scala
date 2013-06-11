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

package es.tid.cosmos.admin.setup

import java.net.URI
import scala.concurrent.Future

import org.mockito.BDDMockito.given
import org.mockito.Matchers.any
import org.mockito.Mockito.{verify, never}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager.{Running, ClusterDescription, ClusterId, ServiceManager}

class SetupCommandTest extends FlatSpec with MustMatchers with MockitoSugar {

  val hdfsId = ClusterId("booya")

  trait WithServiceManager {
    val sm = mock[ServiceManager]
    given(sm.persistentHdfsId).willReturn(hdfsId)
  }

  it must "deploy the persistent HDFS if it hasn't been found" in new WithServiceManager {
    given(sm.describeCluster(any())).willReturn(None)
    given(sm.deployPersistentHdfsCluster()).willReturn(Future.successful())
    new SetupCommand(sm).run() must be (true)
    verify(sm).describeCluster(hdfsId)
    verify(sm).deployPersistentHdfsCluster()
  }

  it must "not deploy the persistent HDFS if it has been found" in new WithServiceManager {
    given(sm.describeCluster(any())).willReturn(Some(new ClusterDescription {
      val id = hdfsId
      val name = ""
      val size = 3
      val state = Running
      val nameNode = new URI("hdfs://host:1234")
    }))
    new SetupCommand(sm).run() must be (true)
    verify(sm).describeCluster(hdfsId)
    verify(sm, never()).deployPersistentHdfsCluster()
  }

  it must "return false is the persistent HDFS deployment fails" in new WithServiceManager {
    given(sm.describeCluster(any())).willReturn(None)
    given(sm.deployPersistentHdfsCluster()).willReturn(
      Future.failed(new Error("Something bad happened")))
    new SetupCommand(sm).run() must be (false)
    verify(sm).describeCluster(hdfsId)
    verify(sm).describeCluster(hdfsId)
  }
}
