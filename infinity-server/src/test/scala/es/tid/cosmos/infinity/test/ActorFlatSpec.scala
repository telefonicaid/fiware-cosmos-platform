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

package es.tid.cosmos.infinity.test

import akka.testkit.TestKit
import akka.actor.ActorSystem
import org.scalatest.{FlatSpecLike, BeforeAndAfterAll}
import org.scalatest.matchers.MustMatchers

/** Base class for testing actors.
  *
  * It guarantees that the actor system is properly shutdown after testing.
  */
abstract class ActorFlatSpec(name: String)
  extends TestKit(ActorSystem(name)) with FlatSpecLike with MustMatchers with BeforeAndAfterAll {

  override def afterAll() = TestKit.shutdownActorSystem(system)
}
