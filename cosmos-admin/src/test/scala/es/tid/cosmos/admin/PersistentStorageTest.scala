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

package es.tid.cosmos.admin

import java.net.URI
import scala.concurrent.Future

import org.mockito.BDDMockito.given
import org.mockito.Mockito.{verify, never}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.clusters._

class PersistentStorageTest extends FlatSpec with MustMatchers with MockitoSugar {

  val hdfsId = ClusterId("booya")

  trait WithServiceManager {
    val sm = mock[ServiceManager]
    given(sm.persistentHdfsId).willReturn(hdfsId)
  }

  trait WithMissingStorage extends WithServiceManager {
    given(sm.describePersistentHdfsCluster()).willReturn(None)
  }

  trait WithExistingStorage extends WithServiceManager {
    given(sm.describePersistentHdfsCluster()).willReturn(Some(new ImmutableClusterDescription(
      id = hdfsId,
      name = "",
      size = 3,
      state = Running,
      nameNode = Some(new URI("hdfs://host:1234")),
      master = Some(HostDetails("host", "ipAddress")),
      slaves  = Seq(HostDetails("host2", "ipAddress2"), HostDetails("host3", "ipAddress3")),
      users = None,
      services = Set("HDFS")
    )))
  }

  it must "deploy the persistent storage if it hasn't been found" in new WithMissingStorage {
    given(sm.describePersistentHdfsCluster()).willReturn(None)
    given(sm.deployPersistentHdfsCluster()).willReturn(Future.successful())
    PersistentStorage.setup(sm) must be (true)
    verify(sm).describePersistentHdfsCluster()
    verify(sm).deployPersistentHdfsCluster()
  }

  it must "not deploy the persistent storage if it has been found" in new WithExistingStorage {
    PersistentStorage.setup(sm) must be (true)
    verify(sm).describePersistentHdfsCluster()
    verify(sm, never()).deployPersistentHdfsCluster()
  }

  it must "return false is the persistent storage deployment fails" in new WithMissingStorage {
    given(sm.describePersistentHdfsCluster()).willReturn(None)
    given(sm.deployPersistentHdfsCluster()).willReturn(
      Future.failed(new Error("Something bad happened")))
    PersistentStorage.setup(sm) must be (false)
    verify(sm).describePersistentHdfsCluster()
  }

  it must "not terminate the persistent storage if it hasn't been found" in new WithMissingStorage {
    given(sm.describePersistentHdfsCluster()).willReturn(None)
    PersistentStorage.terminate(sm) must be (false)
    verify(sm).describePersistentHdfsCluster()
    verify(sm, never()).terminatePersistentHdfsCluster()
  }

  it must "terminate the persistent storage if it has been found" in new WithExistingStorage {
    given(sm.terminatePersistentHdfsCluster()).willReturn(Future.successful())
    PersistentStorage.terminate(sm) must be (true)
    verify(sm).describePersistentHdfsCluster()
  }

  it must "return false is the persistent storage termination fails" in new WithMissingStorage {
    given(sm.describePersistentHdfsCluster()).willReturn(None)
    given(sm.terminatePersistentHdfsCluster()).willReturn(
      Future.failed(new Error("Something bad happened")))
    PersistentStorage.terminate(sm) must be (false)
    verify(sm).describePersistentHdfsCluster()
  }
}
