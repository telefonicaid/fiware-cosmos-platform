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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import org.mockito.BDDMockito._

import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.{Failed, Running, ImmutableClusterDescription}
import es.tid.cosmos.admin.cli.AdminArguments

class CommandRunnerTest extends FlatSpec with MustMatchers with MockitoSugar {

  class WithArguments(arguments: String*) {
    val serviceManager = mock[ServiceManager]
    val clusterDesc = mock[ImmutableClusterDescription]
    val runner= new CommandRunner(new AdminArguments(arguments.toSeq), serviceManager)
  }

  "A command runner" must "exit with non-zero status for invalid arguments" in
    new WithArguments() {
      runner.run() must not be 0
    }

  it must "exit with non-zero status when exceptions are thrown" in
    new WithArguments("setup") {
      given(serviceManager.describePersistentHdfsCluster())
        .willThrow(new UnsupportedOperationException())
      runner.run() must not be 0
    }

  it must "exit with zero status when everything goes OK" in
    new WithArguments("setup") {
      given(clusterDesc.state).willReturn(Running)
      given(serviceManager.describePersistentHdfsCluster())
        .willReturn(Some(clusterDesc))
      runner.run() must be (0)
    }

  it must "exit status non-zero when storage cluster in state failed" in
    new WithArguments("setup") {
      given(clusterDesc.state).willReturn(Failed("state failed"))
      given(serviceManager.describePersistentHdfsCluster())
        .willReturn(Some(clusterDesc))
      runner.run() must be (-1)
    }


  it must "work with multi-subcommand arguments" in
    new WithArguments("persistent-storage", "setup") {
      given(clusterDesc.state).willReturn(Running)
      given(serviceManager.describePersistentHdfsCluster())
        .willReturn(Some(clusterDesc))
      runner.run() must be (0)
    }
}
