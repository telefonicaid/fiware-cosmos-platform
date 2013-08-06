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

package es.tid.cosmos.servicemanager

import java.net.URI
import scala.concurrent.{Future, promise}
import scala.concurrent.duration._

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.platform.ial.MachineState

class MutableClusterDescriptionTest extends FlatSpec with MustMatchers with FutureMatchers {

  val TestTimeout = 1 seconds

  trait SampleMutableClusterDescription {
    private val deploymentPromise = promise[Unit]
    private val machinesPromise = promise[Seq[MachineState]]

    val description = new MutableClusterDescription(
      id = ClusterId(),
      name = "Sample cluster",
      size = 100,
      deployment_> = deploymentPromise.future,
      machines_> = machinesPromise.future,
      nameNode_> = Future.successful(new URI("http://host:8000"))
    ) {
      def finishDeployment() {
        deploymentPromise.success(())
      }
      def finishMachineProvisioning() {
        machinesPromise.success(Seq())
      }

      val runningTrigger_> : Future[Unit] = creation_>
    }
  }

  "A mutable cluster description" must "track cluster state history" in
    new SampleMutableClusterDescription {
      description.state must equal (Provisioning)
      description.stateHistory must equal (Seq(Provisioning))

      description.finishMachineProvisioning()
      description.state must equal (Provisioning)
      description.stateHistory must equal (Seq(Provisioning))

      description.finishDeployment()
      description.runningTrigger_> must runUnder (TestTimeout)
      description.stateHistory must be (Seq(Provisioning, Running))
      description.state must be (Running)
    }

  it must "track cluster state history even if deployment finishes before machine provisioning" in
    new SampleMutableClusterDescription {
      description.state must equal (Provisioning)
      description.stateHistory must equal (Seq(Provisioning))

      description.finishDeployment()
      description.state must equal (Provisioning)
      description.stateHistory must equal (Seq(Provisioning))

      description.finishMachineProvisioning()
      description.runningTrigger_> must runUnder (TestTimeout)
      description.stateHistory must be (Seq(Provisioning, Running))
      description.state must be (Running)
    }
}
