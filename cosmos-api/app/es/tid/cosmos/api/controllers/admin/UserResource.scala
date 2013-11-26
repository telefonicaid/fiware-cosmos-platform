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
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future
import scala.util.Random
import scalaz._

import com.wordnik.swagger.annotations._
import play.Logger
import play.api.libs.json.Json
import play.api.mvc.{Action, Headers}

import es.tid.cosmos.api.auth.{AdminEnabledAuthProvider, MultiAuthProvider}
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile.{Registration, UserId, CosmosProfileDao}
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.api.wizards.{UserUnregistrationWizard, UserRegistrationWizard}

/** Resource for user account administration */
@Api(value = "/admin/v1/user", listingPath = "/doc/admin/v1/user",
  description = "Allow for user account administration")
class UserResource(
    multiUserProvider: MultiAuthProvider,
    serviceManager: ServiceManager,
    dao: CosmosProfileDao,
    override val maintenanceStatus: MaintenanceStatus
  ) extends JsonController with MaintenanceAwareController {

  import Scalaz._

  private type Conn = dao.Conn

  private val registrationWizard = new UserRegistrationWizard(serviceManager)
  private val unregistrationWizard = new UserUnregistrationWizard(serviceManager)

  /** Register a new user account. */
  @ApiOperation(value = "Create a new user account", httpMethod = "POST",
    responseClass = "es.tid.cosmos.api.controllers.admin.RegisterUserResponse")
  @ApiErrors(Array(
    new ApiError(code = 401, reason = "Missing auth header"),
    new ApiError(code = 403, reason = "Forbidden"),
    new ApiError(code = 400, reason = "Invalid JSON payload"),
    new ApiError(code = 409, reason = "Already existing handle"),
    new ApiError(code = 409, reason = "Already existing credentials"),
    new ApiError(code = 500, reason = "Account creation failed"),
    new ApiError(code = 503, reason = "Service is under maintenance")
  ))
  @ApiParamsImplicit(Array(
    new ApiParamImplicit(paramType = "body",
      dataType = "es.tid.cosmos.api.controllers.admin.RegisterUserParams")
  ))
  def register = Action(parse.tolerantJson) { request =>
    for {
      _ <- requireResourceNotUnderMaintenance()
      params <- validJsonBody[RegisterUserParams](request)
      _ <- requireAdminCreds(params.authRealm, request.headers)
      profile <- dao.withTransaction { implicit c =>
        for {
          userId <- uniqueUserId(params)
          handle <- selectHandle(params.handle)
          registration = Registration(handle, params.sshPublicKey, params.email)
        } yield registrationWizard.registerUser(dao, userId, registration)
      }
    } yield Created(Json.toJson(RegisterUserResponse(
      handle = profile.handle,
      apiKey = profile.apiCredentials.apiKey,
      apiSecret = profile.apiCredentials.apiSecret
    )))
  }

  /**
   * Unregister a new user account.
   */
  @ApiOperation(value = "Delete an existing user account", httpMethod = "DELETE",
    responseClass = "String")
  @ApiErrors(Array(
    new ApiError(code = 401, reason = "Missing auth header"),
    new ApiError(code = 403, reason = "Forbidden"),
    new ApiError(code = 404, reason = "User does not exist"),
    new ApiError(code = 500, reason = "Account deletion failed"),
    new ApiError(code = 503, reason = "Service is under maintenance")
  ))
  def unregister(realm: String, id: String) = Action { request =>
    val userId = UserId(realm, id)
    for {
      _ <- requireResourceNotUnderMaintenance()
      _ <- requireAdminCreds(realm, request.headers)
      unregistration_> <- startUnregistration(userId)
    } yield {
      val message = s"User $userId unregistration started"
      Logger.info(message)
      Ok(Json.toJson(Message(message)))
    }
  }

  private def startUnregistration(userId: UserId): ActionValidation[Future[Unit]] = {
    dao.withTransaction { implicit c =>
      for {
        cosmosId <- dao.getProfileId(userId).toSuccess(
          NotFound(Json.toJson(Message(s"User $userId does not exist")))
        )
        unregistration_> <- unregistrationWizard.unregisterUser(dao, cosmosId)
          .leftMap(message => InternalServerError(Json.toJson(message)))
      } yield {
        unregistration_>.onSuccess {
          case _ => Logger.info(s"User with id $cosmosId successfully unregistered")
        }
        unregistration_>.onFailure {
          case ex => Logger.error(s"Could not unregister user with id $cosmosId", ex)
        }
        unregistration_>
      }
    }
  }

  private def selectHandle(reqHandle: Option[String])(implicit c: Conn) =
    reqHandle match {
      case None => Success(generateHandle())
      case Some(handle) if !dao.handleExists(handle) => handle.success
      case Some(handle) => failWith(Conflict, s"Handle '$handle' is already taken")
    }

  private def uniqueUserId(params: RegisterUserParams)
                          (implicit c: Conn): ActionValidation[UserId] = {
    val userId = UserId(params.authRealm, params.authId)
    if (dao.lookupByUserId(userId).isEmpty) userId.success 
    else failWith(Conflict, s"Already existing credentials: $userId")
  }

  private def requireAdminCreds(targetRealm: String, headers: Headers): ActionValidation[Unit] =
    headers.get("Authorization") match {
      case Some(BasicAuth(`targetRealm`, password))
        if canAdministrateUsers(targetRealm, password) => ().success
      case Some(_) => failWith(Forbidden, "Cannot register users")
      case None => failWith(Unauthorized, "Missing authorization header")
    }

  private def canAdministrateUsers(providerName: String, password: String) = (for {
    provider <- multiUserProvider.providers.collectFirst {
      case (`providerName`, adminProvider : AdminEnabledAuthProvider) => adminProvider
    }
  } yield password == provider.adminPassword).getOrElse(false)
  
  private def failWith(status: Status, message: String) = status(Json.toJson(Message(message))).fail

  @tailrec
  private def generateHandle()(implicit c: Conn): String = {
    val handle = s"id${Random.nextLong().abs.toString}"
    if (dao.handleExists(handle)) generateHandle()
    else handle
  }
}
