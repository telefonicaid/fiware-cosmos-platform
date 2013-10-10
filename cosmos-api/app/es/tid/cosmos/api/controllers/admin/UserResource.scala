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

package es.tid.cosmos.api.controllers.admin

import scala.annotation.tailrec
import scala.util.Random

import com.wordnik.swagger.annotations._
import play.api.libs.json.Json
import play.api.mvc.{Headers, Result, Action}

import es.tid.cosmos.api.auth.{AuthProvider, EnabledAdmin, MultiAuthProvider}
import es.tid.cosmos.api.controllers._
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.pages.CosmosProfile
import es.tid.cosmos.api.profile.{Registration, UserId, CosmosProfileDao}
import es.tid.cosmos.servicemanager.ServiceManager

/**
 * Resource for user account administration
 */
@Api(value = "/admin/v1/user", listingPath = "/doc/admin/v1/user",
  description = "Allow for user account administration")
class UserResource(
    multiUserProvider: MultiAuthProvider,
    serviceManager: ServiceManager,
    dao: CosmosProfileDao
  ) extends JsonController {

  private type Conn = dao.Conn

  /**
   * Register a new user account.
   */
  @ApiOperation(value = "Create a new user account", httpMethod = "POST",
    responseClass = "es.tid.cosmos.api.controllers.admin.RegisterUserResponse")
  @ApiErrors(Array(
    new ApiError(code = 401, reason = "Missing auth header"),
    new ApiError(code = 403, reason = "Forbidden"),
    new ApiError(code = 400, reason = "Invalid JSON payload"),
    new ApiError(code = 400, reason = "Already existing handle"),
    new ApiError(code = 400, reason = "Already existing credentials"),
    new ApiError(code = 500, reason = "Account creation failed")
  ))
  @ApiParamsImplicit(Array(
    new ApiParamImplicit(paramType = "body",
      dataType = "es.tid.cosmos.api.controllers.admin.RegisterUserParams")
  ))
  def register = Action(parse.tolerantJson) { request =>
    whenValid[RegisterUserParams](request.body) { params =>
      withAdminCredsFor(params.authRealm, request.headers) {
        dao.withTransaction { implicit c =>
          (for {
            userId <- uniqueUserId(params).right
            handle <- selectHandle(params.handle).right
          } yield createUserAccount(userId, handle, params.sshPublicKey)) match {
            case Left(message) => BadRequest(Json.toJson(message))
            case Right(cosmosProfile) => Created(Json.toJson(RegisterUserResponse(
              handle = cosmosProfile.handle,
              apiKey = cosmosProfile.apiCredentials.apiKey,
              apiSecret = cosmosProfile.apiCredentials.apiSecret
            )))
          }
        }
      }
    }
  }

  private def selectHandle(reqHandle: Option[String])(implicit c: Conn): Either[Message, String] =
    reqHandle match {
      case None => Right(generateHandle())
      case Some(handle) =>
        if (dao.handleExists(handle)) Left(Message(s"Handle '$handle' is already taken"))
        else Right(handle)
    }

  private def uniqueUserId(params: RegisterUserParams)(implicit c: Conn): Either[Message, UserId] = {
    val userId = UserId(params.authRealm, params.authId)
    if (dao.lookupByUserId(userId).isDefined) Left(Message(s"Already existing credentials: $userId"))
    else Right(userId)
  }

  private def withAdminCredsFor(targetRealm: String, headers: Headers)(action: => Result) =
    headers.get("Authorization") match {
      case Some(BasicAuth(`targetRealm`, password))
        if canRegisterUsers(targetRealm, password) => action
      case Some(_) => Forbidden(Json.toJson(Message("Cannot register users")))
      case None => Unauthorized(Json.toJson(Message("Missing authorization header")))
    }

  private def canRegisterUsers(providerName: String, password: String) = (for {
    provider <- multiUserProvider.providers.get(providerName)
    actualPassword <- provider.adminApi match {
      case EnabledAdmin(p) => Some(p)
      case _ => None
    }
  } yield password == actualPassword).getOrElse(false)

  private def createUserAccount(userId: UserId, handle: String, publicKey: String)(implicit c: Conn) = {
    val p = dao.registerUserInDatabase(userId, Registration(handle, publicKey))
    serviceManager.addUsers(serviceManager.persistentHdfsId, p.toClusterUser)
    p
  }

  @tailrec
  private def generateHandle()(implicit c: Conn): String = {
    val handle = s"id-${Random.nextLong().abs.toString}"
    if (dao.handleExists(handle)) generateHandle()
    else handle
  }
}
