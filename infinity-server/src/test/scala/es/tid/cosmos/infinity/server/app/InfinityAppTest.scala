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

package es.tid.cosmos.infinity.server.app

import java.net.InetAddress

import akka.actor._
import akka.testkit.TestProbe
import spray.http._
import spray.http.HttpHeaders.{Authorization, `Remote-Address`}

import es.tid.cosmos.infinity.server.actions.Action
import es.tid.cosmos.infinity.server.authentication.{UserCredentials, AuthenticationComponent}
import es.tid.cosmos.infinity.server.processors.{Request, RequestProcessorComponent}
import es.tid.cosmos.infinity.server.util.Path
import es.tid.cosmos.infinity.test.{ActorFlatSpec, MockActor}

class InfinityAppTest extends ActorFlatSpec("InfinityAppTest") {
  val probe = TestProbe()
  val mockProbe = TestProbe()
  val app = system.actorOf(TestInfinityAppComponent.infinityAppProps)
  var servicesCreated: Seq[MockActor.Created] = _
  val remoteAddress = InetAddress.getByName("10.0.0.1")
  val httpRequest = HttpRequest(
    method = HttpMethods.GET,
    uri = Uri("/infinityfs/v1/metadata/tmp/foo")
  ).withHeaders(List(
    `Remote-Address`(RemoteAddress(remoteAddress)),
    Authorization(BasicHttpCredentials("key", "secret"))
  ))


  object TestInfinityAppComponent extends InfinityAppComponent with AuthenticationComponent
    with RequestProcessorComponent{
    override val authenticationProps = MockActor.props("authentication", probe)
    override def requestProcessorProps(authenticationProps: Props) =
      MockActor.props("request", mockProbe)
  }

  "An infinity app" must "spawn a request processor per request" in {
    probe.send(app, httpRequest)
    mockProbe.expectMsgClass(classOf[MockActor.Created])
  }

  it must "delegate requests to it" in {
    val request = mockProbe.expectMsgPF() {
      case MockActor.Received(r: Request, `app`) => r
    }
    request.action must be (Action.GetMetadata(Path.absolute("/tmp/foo")))
    request.credentials must be (UserCredentials("key", "secret"))
  }
}
