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

import java.net.{InetAddress, InetSocketAddress}
import scala.concurrent.Await
import scala.concurrent.duration._

import akka.io.IO
import akka.actor._
import akka.pattern.ask
import akka.testkit.TestProbe
import akka.util.Timeout
import com.typesafe.config.ConfigFactory
import org.scalatest.matchers._
import spray.can.Http
import spray.http._
import spray.httpx.RequestBuilding

import es.tid.cosmos.infinity.server.authentication._
import es.tid.cosmos.infinity.server.authorization._
import es.tid.cosmos.infinity.server.config.ServiceConfig
import es.tid.cosmos.infinity.test.ActorFlatSpec

class RequestProcessorTest extends ActorFlatSpec("RequestProcessorTest") {
  import AuthenticationProvider._
  import AuthorizationProvider._

  "Request processor" must "request authentication to its provider" in new SampleProcessor {
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

  it must "request authorization after successful authentication" in new SampleProcessor {
    shouldAuthorize()
  }

  it must "return Forbidden on lack of permissions and stop" in new SampleProcessor {
    onceAuthorizationFailed(new AuthorizationException("you lack the permissions")) {
      expectHttpResponse(StatusCodes.Forbidden)
      expectTermination()
    }
  }

  it must "return InternalServerError on unexpected error while authorizing and stop" in
    new SampleProcessor {
      onceAuthorizationFailed(new Error("unexpected error")) {
        expectHttpResponse(StatusCodes.InternalServerError)
        expectTermination()
      }
    }

  it must "return TemporaryRedirect on success authentication and authorization and stop" in
    new SampleProcessor {
      withRedirectionServer { onceAuthorizationSucceeds {
        expectHttpResponse(have(status(StatusCodes.TemporaryRedirect)) and be(ValidRedirection))
        expectTermination()
      }}
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

  it must "timeout after authorization and a finite delay" in
    new SampleProcessor(requestTimeout = 500.millis) {
      shouldAuthorize()
      expectTermination()
    }

  val sampleUri = "/webhdfs/v1/foo/bar?op=OPEN&api.key=user-key&api.secret=user-secret"
  val datanodeService = "datanode1"
  val tokenGenerator = TokenGenerator("c0sm0s")

  def withMappedDataNodeUri(service: String, uri: Uri): Uri = {
    val infUri = withInfinityUri(service, uri)
    val expire = 123456789
    val token = tokenGenerator.encode(infUri, expire)
    infUri.withPath(Uri.Path(s"/p/$token/$expire") ++ infUri.path)
  }

  def withInfinityUri(service: String, uri: Uri): Uri = {
    val cfg = ServiceConfig(service).get
    uri.copy(authority = Uri.Authority(Uri.Host(cfg.infinityHostname), cfg.infinityPort))
  }

  def withWebHdfsUri(service: String, uri: Uri): Uri = {
    val cfg = ServiceConfig(service).get
    uri.copy(authority = Uri.Authority(Uri.Host(cfg.webhdfsHostname), cfg.webhdfsPort))
  }

  class SampleProcessor(requestTimeout: FiniteDuration = 1.minute) extends RequestBuilding {
    val remoteAddress = InetAddress.getLocalHost
    val requester = TestProbe()
    val authenticator = TestProbe()
    val authorizator = TestProbe()
    val credentials = UserCredentials("user-key", "user-secret")
    val profile = UserProfile(
      username = "apv",
      group = "cosmos",
      unixPermissionMask = UnixFilePermissions.fromOctal("777")
    )
    val request = Request(remoteAddress, requester.ref, Get(sampleUri))
    val processorConfig = ConfigFactory.parseString(s"""
      cosmos.infinity.server.request-timeout = ${requestTimeout.toMillis} ms
    """).withFallback(system.settings.config)
    val processor = system.actorOf(
      RequestProcessor.props(authenticator.ref, authorizator.ref, processorConfig)
    )
    requester.watch(processor)

    def shouldAuthenticate(): Unit = {
      processor ! request
      authenticator.expectMsg(Authenticate(UserCredentials("user-key", "user-secret")))
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

    def shouldAuthorize(): Unit = onceAuthenticationSucceeds {
      authorizator.expectMsg(Authorize(request.action, profile))
    }

    def onceAuthorizationSucceeds(body: => Unit): Unit = {
      shouldAuthorize()
      authorizator.send(processor, Authorized)
      body
    }

    def onceAuthorizationFailed(error: Throwable)(body: => Unit): Unit = {
      shouldAuthorize()
      authorizator.send(processor, AuthorizationFailed(error))
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

    def withRedirectionServer(body: => Unit): Unit = {
      implicit val bindTimeout = Timeout(5.seconds)

      val server = system.actorOf(Props(new Actor with ActorLogging {
        override def receive = {
          case Http.Connected(_, _) =>
            sender ! Http.Register(handler = context.self)
          case HttpRequest(HttpMethods.GET, uri, _, _, _) =>
            sender ! HttpResponse(
              status = StatusCodes.TemporaryRedirect,
              headers = List(
                HttpHeaders.Location(withWebHdfsUri(datanodeService, uri)))
            )
          case msg =>
            log.error(s"unexpected message arrived: $msg")
        }
      }))
      val bindProbe = TestProbe()
      bindProbe.send(IO(Http), Http.Bind(server, interface = "localhost", port = 8008))
      bindProbe.expectMsg(Http.Bound(new InetSocketAddress("localhost", 8008)))
      val listener = bindProbe.sender()

      try { body }
      finally {
        bindProbe.send(listener, Http.Unbind)
        bindProbe.expectMsg(Http.Unbound)
      }
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
