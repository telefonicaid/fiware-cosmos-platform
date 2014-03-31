package es.tid.cosmos.infinity.server.authentication.cosmosapi

import scala.concurrent.duration.FiniteDuration
import scala.util.{Failure, Success}

import akka.actor._
import akka.io.IO
import spray.can.Http
import spray.http._
import spray.httpx.RequestBuilding

import es.tid.cosmos.infinity.server.authentication._

private[cosmosapi] class CosmosApiAuthentication(config: CosmosApiAuthentication.Configuration)
  extends Actor with ActorLogging with RequestBuilding {

  import Authentication._
  import CosmosApiAuthentication._

  override def receive = {
    case Authenticate(credentials) =>
      IO(Http)(context.system) ! Http.Connect(
        host = config.authResourceUri.authority.host.address,
        port = config.authResourceUri.authority.port
      )
      context.become(receiveConnected(sender(), credentials))
  }

  private def receiveConnected(requestedBy: ActorRef, credentials: Credentials): Receive = {
    case Http.Connected(_, _) =>
      sender ! requestAuthentication(credentials)
      context.setReceiveTimeout(config.requestTimeout)
      context.become(receiveResponse(requestedBy, credentials))
    case Http.CommandFailed(_) =>
      requestedBy ! AuthenticationFailed(ConnectionException(config.authResourceUri))
      terminate()
  }

  private def receiveResponse(requestedBy: ActorRef, credentials: Credentials): Receive = {
    case HttpResponse(StatusCodes.OK, entity, _, _) =>
      ResponseObject.extractFrom(entity) match {
        case Success(response) =>
          if (authorizedAccess(credentials, response))
            requestedBy ! Authenticated(response.toUserProfile)
          else
            requestedBy ! AuthenticationFailed(InvalidCredentialsException(credentials))
        case Failure(error) =>
          requestedBy ! AuthenticationFailed(error)
      }
      terminate()
    case HttpResponse(StatusCodes.NotFound, _, _, _) =>
      requestedBy ! AuthenticationFailed(InvalidCredentialsException(credentials))
      terminate()
    case HttpResponse(statusCode, _, _, _) =>
      requestedBy ! AuthenticationFailed(StatusException(statusCode))
      terminate()
    case ReceiveTimeout =>
      requestedBy ! AuthenticationFailed(TimeoutException(config.requestTimeout.toMillis))
      terminate()
  }

  private def authorizedAccess(
      credentials: Credentials, response: ResponseObject): Boolean = credentials match {
    case ClusterCredentials(host, _) if response.origins.isDefined =>
      response.origins.get.contains(host)
    case _ =>
      true
  }

  private def requestAuthentication(credentials: Credentials): HttpRequest = credentials match {
    case UserCredentials(key, secret) => requestWithParams(
      "apiKey" -> key,
      "apiSecret" -> secret
    )
    case ClusterCredentials(_, secret) => requestWithParams("clusterSecret" -> secret)
  }

  private def requestWithParams(params: (String, String)*): HttpRequest =
    Get(config.authResourceUri.withQuery(params: _*))

  private def terminate() = context.stop(self)
}

object CosmosApiAuthentication {

  case class ConnectionException(apiUri: Uri) extends Exception(
    s"cannot connect to API server on $apiUri")

  case class StatusException(status: StatusCode) extends Exception(
    s"API responded with unexpected status $status")

  case class TimeoutException(millis: Long) extends Exception(
    s"the API didn't respond before $millis milliseconds")

  case class Configuration(
    authResourceUri: Uri,
    requestTimeout: FiniteDuration
  )

  def props(config: Configuration) = Props(new CosmosApiAuthentication(config))
}
