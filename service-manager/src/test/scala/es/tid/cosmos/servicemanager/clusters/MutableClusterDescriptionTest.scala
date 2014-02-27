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

import java.net.URI
import scala.concurrent.Future
import scala.concurrent.duration._
import scala.language.postfixOps

import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.{ClusterUser, ClusterName}

class MutableClusterDescriptionTest
  extends FlatSpec with MustMatchers with FutureMatchers with OneInstancePerTest {

  class TestDescription extends MutableClusterDescription {
    override val id = ClusterId()
    override var state: ClusterState = Running
    override val size = 2
    override var name = ClusterName("test-description")
    override def services_=(services: Set[String]): Unit = ???
    override def services: Set[String] = ???
    override def users_=(users: Set[ClusterUser]): Unit = ???
    override def users: Option[Set[ClusterUser]] = ???
    override def slaves_=(slaves: Seq[HostDetails]): Unit = ???
    override def slaves: Seq[HostDetails] = ???
    override def master_=(master: HostDetails): Unit = ???
    override def master: Option[HostDetails] = ???
    override def nameNode_=(nameNode: URI): Unit = ???
    override def nameNode: Option[URI] = ???
  }

  "A mutable cluster description" must "be able to guard against failures" in {
    val error = new IllegalArgumentException("some error")
    val description = new TestDescription()
    description.state must not be Failed(error)
    val handledFailure_> = description.withFailsafe {
      Future.failed(error)
    }
    handledFailure_> must runUnder (0.2 seconds)
    handledFailure_> must eventuallyFailWith [IllegalArgumentException]
    description.state must be (Failed(error))
  }

  it must "leave the description unaltered if no failure happens in a failsafe guard" in {
    val description = new TestDescription()
    val previousState = description.state
    val handledFailure_> = description.withFailsafe {
      Future.successful(8)
    }
    handledFailure_> must runUnder (0.2 seconds)
    handledFailure_> must eventually (be (8))
    description.state must be (previousState)
  }
}
