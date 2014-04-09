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
import spray.http.HttpHeaders.`Remote-Address`

import es.tid.cosmos.infinity.server.authentication.AuthenticationComponent
import es.tid.cosmos.infinity.server.authorization.AuthorizationComponent
import es.tid.cosmos.infinity.server.processors.{Request, RequestProcessorComponent}
import es.tid.cosmos.infinity.test.{ActorFlatSpec, MockActor}

class InfinityAppTest extends ActorFlatSpec("InfinityAppTest") {
  val probe = TestProbe()
  val app = system.actorOf(TestInfinityAppComponent.infinityAppProps)
  var servicesCreated: Seq[MockActor.Created] = _
  val remoteAddress = InetAddress.getByName("10.0.0.1")
  val httpRequest = HttpRequest(
    method = HttpMethods.DELETE,
    uri = Uri("/webhdfs/v1/tmp/foo?op=DELETE&recursive=true")
  ).withHeaders(List(`Remote-Address`(RemoteAddress(remoteAddress))))


  object TestInfinityAppComponent extends InfinityAppComponent with AuthenticationComponent
    with AuthorizationComponent with RequestProcessorComponent{
    override val authenticationProps = MockActor.props("authentication", probe)
    override val authorizationProps = MockActor.props("authorization", probe)
    override def requestProcessorProps(
      authenticationProps: Props, authorizationProps: Props) = MockActor.props("request", probe)
  }

  "An infinity app" must "spawn a request processor per request" in {
    probe.send(app, httpRequest)
    probe.expectMsgClass(classOf[MockActor.Created])
  }

  it must "delegate requests to it" in {
    val request = probe.expectMsgPF() {
      case MockActor.Received(r: Request, `app`) => r
    }
    request.remoteAddress must be (remoteAddress)
    request.httpRequest must be (httpRequest)
  }
}
