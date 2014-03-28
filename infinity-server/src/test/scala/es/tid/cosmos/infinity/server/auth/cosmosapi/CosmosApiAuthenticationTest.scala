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

package es.tid.cosmos.infinity.server.auth.cosmosapi

import java.net.InetSocketAddress
import scala.concurrent.duration._

import akka.actor.{Actor, ActorLogging, Props}
import akka.io.IO
import akka.testkit.TestProbe
import akka.util.Timeout
import org.scalatest.matchers.MustMatchers
import spray.can.Http
import spray.http._
import spray.httpx.ResponseTransformation

import es.tid.cosmos.infinity.server.authentication._
import es.tid.cosmos.infinity.server.authentication.cosmosapi.CosmosApiAuthentication
import es.tid.cosmos.infinity.server.authorization.UnixFilePermissions
import es.tid.cosmos.infinity.test.ActorFlatSpec

class CosmosApiAuthenticationTest extends ActorFlatSpec("CosmosApiAuthenticationTest")
  with MustMatchers with ResponseTransformation {

  import Authentication._

  "Authentication provider" must "successfully authenticate with valid user credentials" in new FreshInstance {
    withRunningServer(expectingUserCredentials("api-key", "api-secret")) {
      probe.send(instance, Authenticate(UserCredentials("api-key", "api-secret")))
      probe.expectMsg(Authenticated(UserProfile(
        username = "gandalf",
        group = "istari",
        unixPermissionMask = UnixFilePermissions.fromOctal("777")
      )))
    }
  }

  it must "successfully authenticate with valid cluster credentials from authorized host" in new FreshInstance {
    withRunningServer(expectingClusterCredentials("cluster-secret", "machine-x")) {
      probe.send(instance, Authenticate(ClusterCredentials("machine-x", "cluster-secret")))
      probe.expectMsg(Authenticated(UserProfile(
        username = "gandalf",
        group = "istari",
        unixPermissionMask = UnixFilePermissions.fromOctal("777")
      )))
    }
  }

  it must "fail to authenticate with valid cluster credentials from unauthorized host" in new FreshInstance {
    withRunningServer(expectingClusterCredentials("cluster-secret", "machine-x")) {
      probe.send(instance, Authenticate(ClusterCredentials("machine-y", "cluster-secret")))
      probe.expectMsgPF() {
        case AuthenticationFailed(InvalidCredentialsException(_, _)) => ()
      }
    }
  }

  it must "fail to authenticate when credentials are not valid" in new FreshInstance {
    withRunningServer(expectingUserCredentials("api-key", "api-secret")) {
      probe.send(instance, Authenticate(UserCredentials("other-api-key", "other-api-secret")))
      probe.expectMsgPF() {
        case AuthenticationFailed(InvalidCredentialsException(_, _)) => ()
      }
    }
  }

  it must "fail to authenticate when server responds with unexpected status" in new FreshInstance {
    withRunningServer(responding(StatusCodes.ServiceUnavailable, "unexpected server error")) {
      probe.send(instance, Authenticate(UserCredentials("api-key", "api-secret")))
      probe.expectMsgPF() {
        case AuthenticationFailed(
          CosmosApiAuthentication.StatusException(StatusCodes.ServiceUnavailable)) => ()
      }
    }
  }

  it must "fail to authenticate when server host is unreacheable" in new FreshInstance {
    probe.send(instance, Authenticate(UserCredentials("api-key", "api-secret")))
    probe.expectMsgPF() {
      case AuthenticationFailed(_: CosmosApiAuthentication.ConnectionException) => ()
    }
  }

  it must "fail to authenticate when server request timeouts" in
    new FreshInstance(fastTimeout = true) {
      withRunningServer(notResponding) {
        probe.send(instance, Authenticate(UserCredentials("api-key", "api-secret")))
        probe.expectMsgPF() {
          case AuthenticationFailed(_: CosmosApiAuthentication.TimeoutException) => ()
        }
      }
    }

  type ErrorMessage = String

  private val serverHost = "localhost"
  private val serverPort = 6750

  private def sampleIdentity(whitelist: String*) = {
    val originsLine =
      if (whitelist.isEmpty) "" else s""","origins":["${whitelist.mkString(", ")}"]"""
    HttpEntity(s"""{ "user": "gandalf", "group": "istari", "accessMask": "777" $originsLine }""")
  }

  private def withRunningServer(respond: (Uri) => Option[HttpResponse])(body: => Unit): Unit = {
    implicit val bindTimeout = Timeout(5.seconds)

    val server = system.actorOf(Props(new Actor with ActorLogging {
      override def receive = {
        case Http.Connected(_, _) =>
          sender ! Http.Register(handler = context.self)
        case HttpRequest(HttpMethods.GET, uri, _, _, _) =>
          respond(uri).foreach(sender ! _)
        case msg =>
          log.error(s"unexpected message arrived: $msg")
      }
    }))
    val bindProbe = TestProbe()
    bindProbe.send(IO(Http), Http.Bind(server, interface = serverHost, port = serverPort))
    bindProbe.expectMsg(Http.Bound(new InetSocketAddress(serverHost, serverPort)))
    val listener = bindProbe.sender()
    try {
      body
    } finally {
      bindProbe.send(listener, Http.Unbind)
      bindProbe.expectMsg(Http.Unbound)
    }
  }

  private def expectingUserCredentials(
      apiKey: String, apiSecret: String)(uri: Uri): Option[HttpResponse] = {
    if ((uri.query.get("apiKey") == Some(apiKey)) && (uri.query.get("apiSecret") == Some(apiSecret)))
      responding(StatusCodes.OK, sampleIdentity())(uri)
    else
      responding(StatusCodes.NotFound, "bad credentials")(uri)
  }

  private def expectingClusterCredentials(clusterSecret: String, authorizedHosts: String*)
                                         (uri: Uri): Option[HttpResponse] = {
    if (uri.query.get("clusterSecret") == Some(clusterSecret))
      responding(StatusCodes.OK, sampleIdentity(authorizedHosts: _*))(uri)
    else
      responding(StatusCodes.NotFound, "bad credentials")(uri)
  }

  private def responding(status: StatusCode, entity: HttpEntity)(uri: Uri): Option[HttpResponse] =
    Some(response(status, entity))

  private def notResponding(uri: Uri): Option[HttpResponse] = None

  private def response(status: StatusCode, entity: HttpEntity) = HttpResponse(
    status = status,
    entity = entity
  )

  class FreshInstance(fastTimeout: Boolean = false) {
    val config = CosmosApiAuthentication.Configuration(
      authResourceUri = Uri(s"http://$serverHost:$serverPort/"),
      requestTimeout = if (fastTimeout) 500.millis else 1.minute
    )
    val instance = system.actorOf(CosmosApiAuthentication.props(config))
    val probe = TestProbe()
  }
}
