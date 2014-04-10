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

package es.tid.cosmos.infinity.server.processors

import java.net.InetAddress
import scala.concurrent.duration._

import akka.actor._
import akka.testkit.TestProbe
import com.typesafe.config.ConfigFactory
import org.mockito.BDDMockito.given
import org.scalatest.matchers._
import org.scalatest.mock.MockitoSugar
import spray.http._
import spray.httpx.RequestBuilding

import es.tid.cosmos.infinity.server.actions.Action
import es.tid.cosmos.infinity.server.authentication._
import es.tid.cosmos.infinity.server.config.ServiceConfig
import es.tid.cosmos.infinity.server.permissions.PermissionsMask
import es.tid.cosmos.infinity.test.{ActorFlatSpec, MockActor}

class RequestProcessorTest extends ActorFlatSpec("RequestProcessorTest") with MockitoSugar {
  import Authentication._

  "Request processor" must "instantiate authentication and authorization actors" in
    new SampleProcessor {
      shouldCreateChildActors()
    }

  it must "request authentication to its provider" in new SampleProcessor {
    shouldAuthenticate()
  }

  it must "return Unauthorized on invalid credentials and stop" in new SampleProcessor {
    onceAuthenticationFailed(new AuthenticationException("your credentials are invalid")) {
      expectHttpResponse(StatusCodes.Unauthorized)
      expectTermination()
    }
  }

  it must "return InternalServerError on unexpected error while authenticating and stop" in
    new SampleProcessor {
      onceAuthenticationFailed(new Error("unexpected error")) {
        expectHttpResponse(StatusCodes.InternalServerError)
        expectTermination()
      }
    }

  it must "return action result on success authentication and then stop" in
    new SampleProcessor {
      onceAuthenticationSucceeds {
        expectHttpResponse(have(status(StatusCodes.OK)))
        expectTermination()
      }
    }

  it must "timeout after a finite delay" in new SampleProcessor(requestTimeout = 500.millis) {
    processor ! request
    expectTermination()
  }

  it must "timeout after authentication and a finite delay" in
    new SampleProcessor(requestTimeout = 500.millis) {
      shouldAuthenticate()
      expectTermination()
    }

  val sampleUri = "/webhdfs/v1/foo/bar?op=OPEN"
  val datanodeService = "datanode1"
  val tokenGenerator = TokenGenerator("c0sm0s")

  def withMappedDataNodeUri(service: String, uri: Uri): Uri = {
    val infUri = withInfinityUri(service, uri)
    val expire = 123456789
    val token = tokenGenerator.encode(infUri, expire)
    infUri.withPath(Uri.Path(s"/p/$token/$expire") ++ infUri.path)
  }

  def withInfinityUri(service: String, uri: Uri): Uri = {
    val cfg = ServiceConfig(service, system.settings.config).get
    uri.copy(authority = Uri.Authority(Uri.Host(cfg.infinityHostname), cfg.infinityPort))
  }

  class SampleProcessor(requestTimeout: FiniteDuration = 1.minute) extends RequestBuilding {
    val remoteAddress = InetAddress.getLocalHost
    val requester = TestProbe()
    val authenticator = TestProbe()
    val credentials = UserCredentials("user-key", "user-secret")
    val profile = UserProfile(
      username = "apv",
      group = "cosmos",
      mask = PermissionsMask.fromOctal("777")
    )
    val action = mock[Action]
    given(action.run(profile)).willReturn(HttpResponse(StatusCodes.OK))
    val request = Request(action, credentials, requester.ref)
    val processorConfig = ConfigFactory.parseString(s"""
      cosmos.infinity.server.request-timeout = ${requestTimeout.toMillis} ms
    """).withFallback(system.settings.config)
    val processor = system.actorOf(
      RequestProcessor.props(
        MockActor.props("authenticator", authenticator),
        processorConfig)
    )
    requester.watch(processor)

    def shouldCreateChildActors(): Unit = {
      authenticator.expectMsgPF() { case MockActor.Created("authenticator", _) => () }
    }

    def shouldAuthenticate(): Unit = {
      shouldCreateChildActors()
      processor ! request
      authenticator.expectMsg(MockActor.Received(
        message = Authenticate(UserCredentials("user-key", "user-secret")),
        sender = processor
      ))
    }

    def onceAuthenticationSucceeds(body: => Unit): Unit = {
      shouldAuthenticate()
      authenticator.send(processor, Authenticated(profile))
      body
    }

    def onceAuthenticationFailed(error: Throwable)(body: => Unit): Unit = {
      shouldAuthenticate()
      authenticator.send(processor, AuthenticationFailed(error))
      body
    }

    def expectHttpResponse(statusCode: StatusCode): Unit =
      expectHttpResponse(have(status(statusCode)))

    def expectHttpResponse(matcher: Matcher[HttpResponse]): Unit = {
      requester.expectMsgPF() {
        case rep: HttpResponse => rep
      } must matcher
    }

    def expectTermination() {
      requester.expectTerminated(processor)
    }
  }

  private def status(statusCode: StatusCode) = new HavePropertyMatcher[HttpResponse, StatusCode] {
    override def apply(response: HttpResponse) = HavePropertyMatchResult[StatusCode](
      matches = response.status == statusCode,
      propertyName = "status code",
      expectedValue = statusCode,
      actualValue = response.status
    )
  }

  private object ValidRedirection extends BeMatcher[HttpResponse] {

    private val pathPattern = "/p/(.+)/(\\d+)(/.+)".r

    override def apply(left: HttpResponse) = MatchResult(
      matches = isValidAuthToken(left),
      failureMessage = s"$left is not a valid redirection",
      negatedFailureMessage = s"$left is a valid redirection"
    )

    private def isValidAuthToken(rep: HttpResponse): Boolean = try {
      val location = rep.header[HttpHeaders.Location].get.uri
      val pathPattern(token, expire, path) = location.path.toString()
      val originalPath = location.withPath(Uri.Path(path))
      tokenGenerator.encode(originalPath, expire.toLong) == token
    } catch {
      case e: MatchError => false
    }
  }
}
