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

import akka.actor.{Props, ActorRef, FSM, Actor}
import akka.io.IO
import spray.can.Http
import spray.http._
import spray.httpx.ResponseTransformation

import es.tid.cosmos.infinity.server.auth._
import es.tid.cosmos.infinity.server.config.{ServiceConfig, AuthTokenConfig}

/** Request processor actor class.
  *
  * This actor class encapsulates the business logic that processes an HTTP request received
  * to Infinity Server. The request processor follows a protocol that leads it across the
  * following states.
  *
  * <ul>
  *   <li>Upon creation, it's ready to receive a [[Request]] message that contains all the
  *   relevant information to process the request.
  *   <li>The processor requests its [[AuthenticationProvider]] to authenticate the requester.
  *   <li>The processor requests its [[AuthorizationProvider]] to authorize the action.
  *   <li>The processor submits the request to the appropriate HDFS name node and receives the
  *   response, which is intercepted and modified to reflect the appropriate redirection to
  *   the data node proxy with the corresponding authentication token.
  * </ul>
  *
  * @param authenticationProvider the actor that provides the authentication logic
  * @param authorizationProvider the actor that provides the authorization logic
  */
class RequestProcessor(
    authenticationProvider: ActorRef,
    authorizationProvider: ActorRef) extends Actor
  with FSM[RequestProcessor.StateName, RequestProcessor.StateData]
  with ResponseTransformation {

  import RequestProcessor._
  import AuthenticationProvider._
  import AuthorizationProvider._
  import context.system

  val authTokenConfig = AuthTokenConfig.fromConfig(context.system.settings.config)
  val serviceConfig = ServiceConfig.active
  val tokenGenerator = TokenGenerator(authTokenConfig)

  startWith(Ready, NoData)

  when(Ready) {
    case Event(req: Request, _) =>
      authenticationProvider ! Authenticate(req.credentials)
      goto(Authenticating) using UnauthenticatedRequest(req)
  }

  when (Authenticating) {
    case Event(Authenticated(profile), UnauthenticatedRequest(req)) =>
      authorizationProvider ! Authorize(req.action, profile)
      goto(Authorizing) using AuthenticatedRequest(req, profile)
    case Event(AuthenticationFailed(error), UnauthenticatedRequest(req)) =>
      error match {
        case _: AuthenticationException => req.requester ! HttpResponse(
          status = StatusCodes.Unauthorized,
          entity = HttpEntity(error.toString)
        )
        case _ => req.requester ! HttpResponse(
          status = StatusCodes.InternalServerError,
          entity = HttpEntity(error.toString)
        )
      }
      goto(Ready) using NoData
  }

  when (Authorizing) {
    case Event(Authorized, AuthenticatedRequest(req, _)) =>
      IO(Http)(context.system) ! Http.Connect(
        host = serviceConfig.webhdfsHostname,
        port = serviceConfig.webhdfsPort)
      goto(Forwarding)
    case Event(AuthorizationFailed(error), AuthenticatedRequest(req, _)) =>
      error match {
        case _: AuthorizationException => req.requester ! HttpResponse(
          status = StatusCodes.Forbidden,
          entity = HttpEntity(error.toString)
        )
        case _ => req.requester ! HttpResponse(
          status = StatusCodes.InternalServerError,
          entity = HttpEntity(error.toString)
        )
      }
      goto(Ready) using NoData
  }

  when (Forwarding) {
    case Event(Http.Connected(_, _), AuthenticatedRequest(req, _)) =>
      sender ! req.httpRequest
      stay()
    case Event(rep: HttpResponse, AuthenticatedRequest(req, _)) =>
      req.requester ! mapResponse(rep)
      goto(Ready) using NoData
    case Event(rep: Http.ConnectionClosed, AuthenticatedRequest(req, _)) =>
      req.requester ! rep
      goto(Ready) using NoData
  }

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
    val expireOn = (System.currentTimeMillis() / 1000) + authTokenConfig.duration
    uri.withPath(Uri.Path(authTokenConfig.pathTemplate.toString
      .replace("${token}", tokenGenerator.encode(uri, expireOn))
      .replace("${expire}", expireOn.toString)
      .replace("${path}", uri.path.toString())))
  }
}

object RequestProcessor {

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
  def props(authenticationProvider: ActorRef, authorizationProvider: ActorRef): Props =
    Props(classOf[RequestProcessor], authenticationProvider, authorizationProvider)
}
