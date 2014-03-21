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

package es.tid.cosmos.admin.cluster

import java.net.URI
import scala.concurrent.Future

import org.mockito.BDDMockito.given
import org.mockito.Mockito.{verify, never}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.clusters._

class DefaultClusterCommandsTest extends FlatSpec with MustMatchers with MockitoSugar {

  val clusterId = ClusterId("booya")

  trait WithServiceManager {
    val sm = mock[ServiceManager]
    val commands = new DefaultClusterCommands(sm)
  }

  trait WithMissingStorage extends WithServiceManager {
    given(sm.describeCluster(clusterId)).willReturn(None)
  }

  trait WithExistingStorage extends WithServiceManager {
    given(sm.describeCluster(clusterId)).willReturn(Some(new ImmutableClusterDescription(
      id = clusterId,
      name = ClusterName(""),
      state = Running,
      size = 3,
      nameNode = Some(new URI("hdfs://host:1234")),
      master = Some(HostDetails("host", "ipAddress")),
      slaves  = Seq(HostDetails("host2", "ipAddress2"), HostDetails("host3", "ipAddress3")),
      users = None,
      services = Set("HDFS")
    )))
  }

  it must "not terminate the cluster if it hasn't been found" in new WithMissingStorage {
    commands.terminate(clusterId) must not be 'success
    verify(sm).describeCluster(clusterId)
    verify(sm, never()).terminateCluster(clusterId)
  }

  it must "terminate the cluster if it has been found" in new WithExistingStorage {
    given(sm.terminateCluster(clusterId)).willReturn(Future.successful())
    commands.terminate(clusterId) must be ('success)
    verify(sm).describeCluster(clusterId)
  }
}
