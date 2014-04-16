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

import com.typesafe.config.ConfigFactory
import org.mockito.BDDMockito.given
import org.mockito.Matchers.any
import org.mockito.Mockito.{never, verify}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.clusters._
import es.tid.cosmos.servicemanager.services.InfinityServer.InfinityServerParameters

class DefaultPersistentStorageCommandsTest extends FlatSpec with MustMatchers with MockitoSugar {

  trait WithServiceManager {
    val sm = mock[ServiceManager]
    val hdfsId = ClusterId("booya")
    given(sm.persistentHdfsId).willReturn(hdfsId)
    val parameters = InfinityServerParameters(
      cosmosApiUrl = "http://base-url",
      infinitySecret = "secret"
    )
    val config = ConfigFactory.parseString(
      s"""
         |application.baseurl="${parameters.cosmosApiUrl}"
         |infinity.secret="${parameters.infinitySecret}"
       """.stripMargin)
    val commands = new DefaultPersistentStorageCommands(sm, config)

    def verifyDeploymentAttempt(): Unit = {
      verify(sm).deployPersistentHdfsCluster(parameters)
    }

    def verifyNoDeploymentAttempt(): Unit = {
      verify(sm, never()).deployPersistentHdfsCluster(any[InfinityServerParameters])
    }

    def verifyTerminationAttempt(): Unit = {
      verify(sm).terminatePersistentHdfsCluster()
    }

    def verifyNoTerminationAttempt(): Unit = {
      verify(sm, never()).terminatePersistentHdfsCluster()
    }
  }

  trait WithMissingStorage extends WithServiceManager {
    given(sm.describePersistentHdfsCluster()).willReturn(None)

    def givenDeploymentWillSucceed(): Unit = {
      given(sm.deployPersistentHdfsCluster(parameters)).willReturn(Future.successful())
    }

    def givenDeploymentWillFail(): Unit = {
      given(sm.deployPersistentHdfsCluster(any[InfinityServerParameters]))
        .willReturn(Future.failed(new Error("Something bad happened")))
    }
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
      services = Set("HDFS"),
      blockedPorts = Set(2, 4, 6)
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
    commands.setup() must be ('success)
    verifyDeploymentAttempt()
  }

  it must "not deploy the persistent storage if it has been found" in new WithStorage {
    givenStorageState(Running)
    commands.setup() must be ('success)
    verifyNoDeploymentAttempt()
  }

  it must "return false is the persistent storage deployment fails" in new WithMissingStorage {
    givenDeploymentWillFail()
    commands.setup() must not be 'success
    verifyDeploymentAttempt()
  }

  it must "not terminate the persistent storage if it hasn't been found" in new WithMissingStorage {
    commands.terminate() must not be 'success
    verifyNoTerminationAttempt()
  }

  it must "terminate the persistent storage if it has been found" in new WithStorage {
    givenStorageState(Running)
    givenTerminationWillSucceed()
    commands.terminate() must be ('success)
    verifyTerminationAttempt()
  }

  it must "return error if persistent storage termination fail" in new WithStorage {
    givenStorageState(Running)
    givenTerminationWillFail()
    commands.terminate() must not be 'success
    verifyTerminationAttempt()
  }

  it must "return error if the persistent storage termination fails" in new WithMissingStorage {
    commands.terminate() must not be 'success
    verifyNoTerminationAttempt()
  }

  it must "not deploy the persistent storage if it is in failed state" in new WithStorage {
    givenStorageState(Failed("some reason"))
    commands.setup() must not be 'success
    verifyNoDeploymentAttempt()
  }

  it must "not deploy the persistent storage if it is in terminating state" in new WithStorage {
    givenStorageState(Terminating)
    commands.setup() must not be 'success
    verifyNoDeploymentAttempt()
  }

  it must "not deploy the persistent storage if it is in terminated state" in new WithStorage {
    givenStorageState(Terminated)
    commands.setup() must not be 'success
    verifyNoDeploymentAttempt()
  }

  it must "not terminate the persistent storage if it is in provisioning state" in new WithStorage {
    givenStorageState(Provisioning)
    commands.terminate() must not be 'success
    verifyNoTerminationAttempt()
  }

  it must "not terminate the persistent storage if it is in terminating state" in new WithStorage {
    givenStorageState(Terminating)
    commands.terminate() must be ('success)
    verifyNoTerminationAttempt()
  }

  it must "not terminate the persistent storage if it is in terminated state" in new WithStorage {
    givenStorageState(Terminated)
    commands.terminate() must be ('success)
    verifyNoTerminationAttempt()
  }

  it must "not terminate the persistent storage if it is in failed state" in new WithStorage {
    givenStorageState(Failed("some reason"))
    commands.terminate() must not be 'success
    verifyNoTerminationAttempt()
  }
}
