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

  trait WithProvisioningStorage extends WithServiceManager {
    given(sm.describePersistentHdfsCluster()).willReturn(Some(new ImmutableClusterDescription(
      id = hdfsId,
      name = "",
      size = 3,
      state = Provisioning,
      nameNode = Some(new URI("hdfs://host:1234")),
      master = Some(HostDetails("host", "ipAddress")),
      slaves  = Seq(HostDetails("host2", "ipAddress2"), HostDetails("host3", "ipAddress3")),
      users = None,
      services = Set("HDFS")
    )))
  }

  trait WithRunningStorage extends WithServiceManager {
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

  trait WithTerminatingStorage extends WithServiceManager {
    given(sm.describePersistentHdfsCluster()).willReturn(Some(new ImmutableClusterDescription(
      id = hdfsId,
      name = "",
      size = 3,
      state = Terminating,
      nameNode = Some(new URI("hdfs://host:1234")),
      master = Some(HostDetails("host", "ipAddress")),
      slaves  = Seq(HostDetails("host2", "ipAddress2"), HostDetails("host3", "ipAddress3")),
      users = None,
      services = Set("HDFS")
    )))
  }

  trait WithTerminatedStorage extends WithServiceManager {
    given(sm.describePersistentHdfsCluster()).willReturn(Some(new ImmutableClusterDescription(
      id = hdfsId,
      name = "",
      size = 3,
      state = Terminated,
      nameNode = Some(new URI("hdfs://host:1234")),
      master = Some(HostDetails("host", "ipAddress")),
      slaves  = Seq(HostDetails("host2", "ipAddress2"), HostDetails("host3", "ipAddress3")),
      users = None,
      services = Set("HDFS")
    )))
  }

  trait WithFailedStorage extends WithServiceManager {
    given(sm.describePersistentHdfsCluster()).willReturn(Some(new ImmutableClusterDescription(
      id = hdfsId,
      name = "",
      size = 3,
      state = Failed("some reason"),
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

  it must "not deploy the persistent storage if it has been found" in new WithRunningStorage {
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

  it must "terminate the persistent storage if it has been found" in new WithRunningStorage {
    given(sm.terminatePersistentHdfsCluster()).willReturn(Future.successful())
    PersistentStorage.terminate(sm) must be (true)
    verify(sm).describePersistentHdfsCluster()
  }

  it must "return false is the persistent storage termination is missing" in new WithMissingStorage {
    given(sm.describePersistentHdfsCluster()).willReturn(None)
    given(sm.terminatePersistentHdfsCluster()).willReturn(
      Future.failed(new Error("Something bad happened")))
    PersistentStorage.terminate(sm) must be (false)
    verify(sm).describePersistentHdfsCluster()
  }

  it must "not deploy the persistent storage if it is in failed state" in new WithFailedStorage {
    PersistentStorage.setup(sm) must be (false)
    verify(sm).describePersistentHdfsCluster()
    verify(sm, never()).deployPersistentHdfsCluster()
  }

  it must "not deploy the persistent storage if it is in terminating state" in new WithTerminatingStorage {
    PersistentStorage.setup(sm) must be (false)
    verify(sm).describePersistentHdfsCluster()
    verify(sm, never()).deployPersistentHdfsCluster()
  }

  it must "not deploy the persistent storage if it is in terminated state" in new WithTerminatedStorage {
    PersistentStorage.setup(sm) must be (false)
    verify(sm).describePersistentHdfsCluster()
    verify(sm, never()).deployPersistentHdfsCluster()
  }

  it must "do not terminate the persistent storage if it is in provisioning state" in new WithProvisioningStorage {
    PersistentStorage.terminate(sm) must be (false)
    verify(sm).describePersistentHdfsCluster()
    verify(sm, never()).terminatePersistentHdfsCluster()
  }

  it must "do not terminate the persistent storage if it is in terminating state" in new WithTerminatingStorage {
    PersistentStorage.terminate(sm) must be (true)
    verify(sm).describePersistentHdfsCluster()
    verify(sm, never()).terminatePersistentHdfsCluster()
  }

  it must "do not terminate the persistent storage if it is in terminated state" in new WithTerminatedStorage {
    PersistentStorage.terminate(sm) must be (true)
    verify(sm).describePersistentHdfsCluster()
    verify(sm, never()).terminatePersistentHdfsCluster()
  }

  it must "do not terminate the persistent storage if it is in failed state" in new WithFailedStorage {
    PersistentStorage.terminate(sm) must be (false)
    verify(sm).describePersistentHdfsCluster()
    verify(sm, never()).terminatePersistentHdfsCluster()
  }

}
