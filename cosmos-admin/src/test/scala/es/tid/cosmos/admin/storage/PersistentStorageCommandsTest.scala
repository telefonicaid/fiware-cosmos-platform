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

package es.tid.cosmos.admin.storage

import java.net.URI
import scala.concurrent.Future

import org.mockito.BDDMockito.given
import org.mockito.Mockito.{verify, never}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.clusters._

class PersistentStorageCommandsTest extends FlatSpec with MustMatchers with MockitoSugar {

  trait WithServiceManager {
    val sm = mock[ServiceManager]
    val hdfsId = ClusterId("booya")
    given(sm.persistentHdfsId).willReturn(hdfsId)

    def verifyDeploymentAttempt(): Unit = verify(sm).deployPersistentHdfsCluster()
    def verifyNoDeploymentAttempt(): Unit = verify(sm, never()).deployPersistentHdfsCluster()

    def verifyTerminationAttempt(): Unit = verify(sm).terminatePersistentHdfsCluster()
    def verifyNoTerminationAttempt(): Unit = verify(sm, never()).terminatePersistentHdfsCluster()
  }

  trait WithMissingStorage extends WithServiceManager {
    given(sm.describePersistentHdfsCluster()).willReturn(None)

    def givenDeploymentWillSucceed(): Unit = given(sm.deployPersistentHdfsCluster())
      .willReturn(Future.successful())

    def givenDeploymentWillFail(): Unit = given(sm.deployPersistentHdfsCluster())
      .willReturn(Future.failed(new Error("Something bad happened")))
  }

  trait WithStorage extends WithServiceManager {
    val clusterDescription = new ImmutableClusterDescription(
      id = hdfsId,
      name = ClusterName(""),
      size = 3,
      state = null,
      nameNode = Some(new URI("hdfs://host:1234")),
      master = Some(HostDetails("host", "ipAddress")),
      slaves  = Seq(HostDetails("host2", "ipAddress2"), HostDetails("host3", "ipAddress3")),
      users = None,
      services = Set("HDFS")
    )

    def givenStorageState(state: ClusterState): Unit = given(sm.describePersistentHdfsCluster())
        .willReturn(Some(clusterDescription.copy(state = state)))

    def givenTerminationWillSucceed(): Unit =
      given(sm.terminatePersistentHdfsCluster()).willReturn(Future.successful())

    def givenTerminationWillFail(): Unit = given(sm.terminatePersistentHdfsCluster())
      .willReturn(Future.failed(new Error("Termination failed")))
  }

  it must "deploy the persistent storage if it hasn't been found" in new WithMissingStorage {
    givenDeploymentWillSucceed()
    PersistentStorageCommands.setup(sm) must be ('success)
    verifyDeploymentAttempt()
  }

  it must "not deploy the persistent storage if it has been found" in new WithStorage {
    givenStorageState(Running)
    PersistentStorageCommands.setup(sm) must be ('success)
    verifyNoDeploymentAttempt()
  }

  it must "return false is the persistent storage deployment fails" in new WithMissingStorage {
    givenDeploymentWillFail()
    PersistentStorageCommands.setup(sm) must not be 'success
    verifyDeploymentAttempt()
  }

  it must "not terminate the persistent storage if it hasn't been found" in new WithMissingStorage {
    PersistentStorageCommands.terminate(sm) must not be 'success
    verifyNoTerminationAttempt()
  }

  it must "terminate the persistent storage if it has been found" in new WithStorage {
    givenStorageState(Running)
    givenTerminationWillSucceed()
    PersistentStorageCommands.terminate(sm) must be ('success)
    verifyTerminationAttempt()
  }

  it must "return error if persistent storage termination fail" in new WithStorage {
    givenStorageState(Running)
    givenTerminationWillFail()
    PersistentStorageCommands.terminate(sm) must not be 'success
    verifyTerminationAttempt()
  }

  it must "return error if the persistent storage termination fails" in new WithMissingStorage {
    PersistentStorageCommands.terminate(sm) must not be 'success
    verifyNoTerminationAttempt()
  }

  it must "not deploy the persistent storage if it is in failed state" in new WithStorage {
    givenStorageState(Failed("some reason"))
    PersistentStorageCommands.setup(sm) must not be 'success
    verifyNoDeploymentAttempt()
  }

  it must "not deploy the persistent storage if it is in terminating state" in new WithStorage {
    givenStorageState(Terminating)
    PersistentStorageCommands.setup(sm) must not be 'success
    verifyNoDeploymentAttempt()
  }

  it must "not deploy the persistent storage if it is in terminated state" in new WithStorage {
    givenStorageState(Terminated)
    PersistentStorageCommands.setup(sm) must not be 'success
    verifyNoDeploymentAttempt()
  }

  it must "not terminate the persistent storage if it is in provisioning state" in new WithStorage {
    givenStorageState(Provisioning)
    PersistentStorageCommands.terminate(sm) must not be 'success
    verifyNoTerminationAttempt()
  }

  it must "not terminate the persistent storage if it is in terminating state" in new WithStorage {
    givenStorageState(Terminating)
    PersistentStorageCommands.terminate(sm) must be ('success)
    verifyNoTerminationAttempt()
  }

  it must "not terminate the persistent storage if it is in terminated state" in new WithStorage {
    givenStorageState(Terminated)
    PersistentStorageCommands.terminate(sm) must be ('success)
    verifyNoTerminationAttempt()
  }

  it must "not terminate the persistent storage if it is in failed state" in new WithStorage {
    givenStorageState(Failed("some reason"))
    PersistentStorageCommands.terminate(sm) must not be 'success
    verifyNoTerminationAttempt()
  }
}
