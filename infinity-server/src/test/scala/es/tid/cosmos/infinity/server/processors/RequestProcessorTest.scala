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

import java.net.InetSocketAddress
import scala.concurrent.Await
import scala.concurrent.duration._

import akka.io.IO
import akka.actor._
import akka.pattern.ask
import akka.testkit.{TestProbe, TestKit}
import akka.util.Timeout
import org.scalatest.{BeforeAndAfterAll, FlatSpec}
import org.scalatest.matchers.{MatchResult, BeMatcher, MustMatchers}
import spray.http._
import spray.can.Http

import es.tid.cosmos.infinity.server.auth._
import es.tid.cosmos.infinity.server.config.ServiceConfig
import es.tid.cosmos.infinity.server.util.Path

class RequestProcessorTest extends TestKit(ActorSystem("RequestProcessorTest"))
    with FlatSpec
    with BeforeAndAfterAll
    with MustMatchers {

  import AuthenticationProvider._
  import AuthorizationProvider._

  override def afterAll() {
    TestKit.shutdownActorSystem(system)
  }

  "Request processor" must "request authentication to its provider" in new SampleProcessor {
    shouldAuthenticate()
  }
  
  it must "return Unauthorized on invalid credentials" in new SampleProcessor {
    onceAuthenticationFailed(new AuthenticationException("your credentials are invalid")) {
      requester.expectMsgPF() {
        case rep: HttpResponse =>
          rep.status must be (StatusCodes.Unauthorized)
      }
    }
  }

  it must "return InternalServerError on unexpected error while authenticating" in new SampleProcessor {
    onceAuthenticationFailed(new Error("unexpected error")) {
      requester.expectMsgPF() {
        case rep: HttpResponse =>
          rep.status must be (StatusCodes.InternalServerError)
      }
    }
  }

  it must "request authorization after successful authentication" in new SampleProcessor {
    shouldAuthorize()
  }

  it must "return Forbidden on lack of permissions" in new SampleProcessor {
    onceAuthorizationFailed(new AuthorizationException("you lack the permissions")) {
      requester.expectMsgPF() {
        case rep: HttpResponse =>
          rep.status must be (StatusCodes.Forbidden)
      }
    }
  }

  it must "return InternalServerError on unexpected error while authorizing" in new SampleProcessor {
    onceAuthorizationFailed(new Error("unexpected error")) {
      requester.expectMsgPF() {
        case rep: HttpResponse =>
          rep.status must be (StatusCodes.InternalServerError)
      }
    }
  }

  it must "return TemporaryRedirect on success authentication and authorization" in new SampleProcessor {
    withRedirectionServer { onceAuthorizationSucceeds {
      requester.expectMsgPF() {
        case rep: HttpResponse =>
          rep.status must be (StatusCodes.TemporaryRedirect)
          rep must be (ValidRedirection)
      }
    }}
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
    val cfg = ServiceConfig(service).get
    uri.copy(authority = Uri.Authority(Uri.Host(cfg.infinityHostname), cfg.infinityPort))
  }

  def withWebHdfsUri(service: String, uri: Uri): Uri = {
    val cfg = ServiceConfig(service).get
    uri.copy(authority = Uri.Authority(Uri.Host(cfg.webhdfsHostname), cfg.webhdfsPort))
  }

  case class SampleRequest(
      credentials: Credentials,
      requester: ActorRef) extends Request {
    private val service = ServiceConfig.active
    override val httpRequest = HttpRequest(
      method = HttpMethods.GET,
      uri = Uri(
        s"http://${service.infinityHostname}:${service.infinityPort}/webhdfs/v1/foo/bar?op=OPEN"),
      headers = List(HttpHeaders.Host(s"${service.infinityHostname}:${service.infinityPort}"))
    )
    override val action = ReadAction(Path.absolute("/foo/bar"))
  }

  trait SampleProcessor {
    val requester = TestProbe()
    val authenticator = TestProbe()
    val authorizator = TestProbe()
    val credentials = UserCredentials("user-key", "user-secret")
    val profile = UserProfile(
      username = "apv",
      group = "cosmos",
      unixPermissionMask = UnixFilePermissions.fromOctal("777")
    )
    val request = SampleRequest(credentials, requester.ref)
    val processor = system.actorOf(RequestProcessor.props(authenticator.ref, authorizator.ref))

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

    def withRedirectionServer(body: => Unit): Unit = {
      implicit val bindTimeout = Timeout(5.seconds)

      val listener = system.actorOf(Props(new Actor with ActorLogging {
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
      val bindResponse = Await.result(
        ask(IO(Http), Http.Bind(listener, interface = "localhost", port = 8008)),
        bindTimeout.duration)
      bindResponse must be (Http.Bound(new InetSocketAddress("localhost", 8008)))
      body
      listener ! Http.Unbind
    }
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
