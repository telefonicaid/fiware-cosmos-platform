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

import java.util.concurrent.TimeUnit
import scala.concurrent.duration._

import akka.actor._
import akka.actor.FSM.{Failure, Normal}
import akka.io.IO
import com.typesafe.config.Config
import spray.can.Http
import spray.http._
import spray.httpx.ResponseTransformation

import es.tid.cosmos.infinity.server.authentication._
import es.tid.cosmos.infinity.server.authorization.{AuthorizationException, Authorization}
import es.tid.cosmos.infinity.server.config.{AuthTokenConfig, ServiceConfig}

/** Request processor actor class.
  *
  * This actor class encapsulates the business logic that processes an HTTP request received
  * to Infinity Server. The request processor follows a protocol that leads it across the
  * following states.
  *
  * <ul>
  *   <li>Upon creation, it's ready to receive a [[Request]] message that contains all the
  *   relevant information to process the request.
  *   <li>The processor requests its [[Authentication]] to authenticate the requester.
  *   <li>The processor requests its [[Authorization]] to authorize the action.
  *   <li>The processor submits the request to the appropriate HDFS name node and receives the
  *   response, which is intercepted and modified to reflect the appropriate redirection to
  *   the data node proxy with the corresponding authentication token.
  *   <li>The processor terminates itself.
  * </ul>
  *
  * This lifecycle is intended for just one request (an instances of RequestProcessor per request).
  *
  * @param authenticationRef  the actor providing the authentication logic
  * @param authorizationRef   the actor that provides the authorization logic
  * @param configuration      actor configuration
  */
private[processors] class RequestProcessor(
    authenticationRef: ActorRef,
    authorizationRef: ActorRef,
    configuration: RequestProcessor.Configuration
  ) extends Actor
  with FSM[RequestProcessor.StateName, RequestProcessor.StateData]
  with ResponseTransformation {

  import RequestProcessor._
  import Authentication._
  import Authorization._

  val tokenGenerator = TokenGenerator(configuration.authTokenConfig)

  startWith(Ready, NoData)

  when(Ready) {
    case Event(req: Request, _) =>
      requestAuthentication(req.credentials)
      scheduleRequestTimeout()
      goto(Authenticating) using UnauthenticatedRequest(req)
  }

  when (Authenticating) {
    case Event(Authenticated(profile), UnauthenticatedRequest(req)) =>
      authorizationRef ! Authorize(req.action, profile)
      goto(Authorizing) using AuthenticatedRequest(req, profile)

    case Event(AuthenticationFailed(error: AuthenticationException), UnauthenticatedRequest(req)) =>
      stopReportingUnauthorized(error, req)

    case Event(AuthenticationFailed(unexpectedError), UnauthenticatedRequest(req)) =>
      stopReportingFailure(unexpectedError, req)
  }

  when (Authorizing) {
    case Event(Authorized, AuthenticatedRequest(req, _)) =>
      IO(Http)(context.system) ! Http.Connect(
        host = configuration.serviceConfig.webhdfsHostname,
        port = configuration.serviceConfig.webhdfsPort)
      goto(Forwarding)

    case Event(AuthorizationFailed(error: AuthorizationException), AuthenticatedRequest(req, _)) =>
      stopReportingForbidden(error, req)

    case Event(AuthorizationFailed(unexpectedError), AuthenticatedRequest(req, _)) =>
      stopReportingFailure(unexpectedError, req)
  }

  when (Forwarding) {
    case Event(Http.Connected(_, _), AuthenticatedRequest(req, _)) =>
      sender ! req.httpRequest
      stay()

    case Event(rep: HttpResponse, AuthenticatedRequest(req, _)) =>
      req.responder ! mapResponse(rep)
      stop(Normal)

    case Event(rep: Http.ConnectionClosed, AuthenticatedRequest(req, _)) =>
      req.responder ! rep
      stop(Normal)
  }

  whenUnhandled {
    case Event(RequestTimeout, _) => stop(Failure(RequestTimeout))
  }

  private def scheduleRequestTimeout(): Unit = {
    import context.dispatcher
    context.system.scheduler.scheduleOnce(
      delay = configuration.requestTimeout,
      receiver = self,
      message = RequestTimeout
    )
  }

  private def requestAuthentication(credentials: Credentials): Unit =
    authenticationRef ! Authenticate(credentials)

  private def mapResponse(rep: HttpResponse): HttpResponse = rep.withHeaders(rep.headers.map {
    case HttpHeaders.Location(uri) => HttpHeaders.Location(mapUri(uri))
    case header => header
  })

  private def mapUri(uri: Uri): Uri = {
    val host = uri.authority.host.address
    val port = uri.authority.port
    val srv = ServiceConfig.fromWebHdfs(host, port)(context.system).getOrElse(
      throw new IllegalStateException(s"no service defined for WebHDFS endpoint $host:$port"))
    val redirectionUri = uri
      .withHost(srv.infinityHostname)
      .withPort(srv.infinityPort)
    tokenizePath(redirectionUri)
  }

  private def tokenizePath(uri: Uri): Uri = {
    val expireOn = (System.currentTimeMillis() / 1000) + configuration.authTokenConfig.duration
    uri.withPath(Uri.Path(configuration.authTokenConfig.pathTemplate.toString
      .replace("${token}", tokenGenerator.encode(uri, expireOn))
      .replace("${expire}", expireOn.toString)
      .replace("${path}", uri.path.toString())))
  }

  private def stopReportingFailure(error: Throwable, req: Request): State = {
    reportError(StatusCodes.InternalServerError, error, req)
    stop(Failure(error))
  }

  private def stopReportingForbidden(error: AuthorizationException, req: Request): State = {
    reportError(StatusCodes.Forbidden, error, req)
    stop(Normal)
  }

  private def stopReportingUnauthorized(error: AuthenticationException, req: Request): State = {
    reportError(StatusCodes.Unauthorized, error, req)
    stop(Normal)
  }

  private def reportError(status: StatusCode, error: Throwable, req: Request): Unit =
    req.responder ! HttpResponse(status = status, entity = HttpEntity(error.toString))
}

object RequestProcessor {

  case class Configuration(
    requestTimeout: FiniteDuration,
    authTokenConfig: AuthTokenConfig,
    serviceConfig: ServiceConfig
  )

  object Configuration {
    def fromSystemConfig(systemConfig: Config): Configuration = Configuration(
      requestTimeout = systemConfig.getDuration(RequestTimeoutProperty, TimeUnit.MILLISECONDS).millis,
      authTokenConfig = AuthTokenConfig.fromConfig(systemConfig),
      serviceConfig = ServiceConfig.active(systemConfig)
    )
  }

  /** The state of a living processor. */
  sealed trait StateName

  /** The processor is ready to attempt a new request. */
  case object Ready extends StateName

  /** The processor is authenticating the request against an authentication provider. */
  case object Authenticating extends StateName

  /** The processor is authorizing the request against an authorization provider. */
  case object Authorizing extends StateName

  /** The processor is forwarding the request to the HDFS endpoint. */
  case object Forwarding extends StateName

  /** The data maintained by the request processor. */
  sealed trait StateData

  /** No data maintained by the request processor. */
  case object NoData extends StateData

  /** Unauthenticated request maintained by the request processor. */
  case class UnauthenticatedRequest[R <: Request](req: R) extends StateData

  /** Authenticated request maintained by the request processor. */
  case class AuthenticatedRequest[R <: Request](req: R, profile: UserProfile) extends StateData

  /** Obtain the props object of a [[RequestProcessor]] from its construction params. */
  def props(authenticationRef: ActorRef, authorizationRef: ActorRef, config: Config): Props =
    Props(new RequestProcessor(authenticationRef, authorizationRef,
      Configuration.fromSystemConfig(config)))

  private val RequestTimeoutProperty = "cosmos.infinity.server.request-timeout"

  private case object RequestTimeout
}
