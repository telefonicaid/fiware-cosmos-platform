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

package es.tid.cosmos.servicemanager.clusters

import scala.concurrent.Future
import scala.concurrent.duration._
import scala.language.postfixOps

import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.ambari.InMemoryClusterDescription

class MutableClusterDescriptionTest
  extends FlatSpec with MustMatchers with FutureMatchers with OneInstancePerTest {

  val description: MutableClusterDescription = new InMemoryClusterDescription(
    ClusterId(), clusterName = "test", clusterSize = 4)

  "A mutable cluster description" must "be able to guard against failures" in {
    val error = new IllegalArgumentException("some error")
    description.state must not be Failed(error)
    val handledFailure_> = description.withFailsafe {
      Future.failed(error)
    }
    handledFailure_> must runUnder (0.2 seconds)
    handledFailure_> must eventuallyFailWith [IllegalArgumentException]
    description.state must be (Failed(error))
  }

  it must "leave the description unaltered if no failure happens in a failsafe guard" in {
    val previousState = description.state
    val handledFailure_> = description.withFailsafe {
      Future.successful(8)
    }
    handledFailure_> must runUnder (0.2 seconds)
    handledFailure_> must eventually (be (8))
    description.state must be (previousState)
  }
}
