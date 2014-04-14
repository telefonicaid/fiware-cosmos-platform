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

import javax.ws.rs.PathParam
import scala.annotation.tailrec
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future
import scala.util.Random
import scalaz._

import com.wordnik.swagger.annotations._
import play.Logger
import play.api.libs.json.Json
import play.api.mvc.{Action, Controller, Headers}

import es.tid.cosmos.api.auth.multiauth.MultiAuthProvider
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile.{Registration, UserId}
import es.tid.cosmos.api.profile.dao._
import es.tid.cosmos.api.wizards.{UserRegistrationWizard, UserUnregistrationWizard}
import es.tid.cosmos.common.BasicAuth
import es.tid.cosmos.servicemanager.ServiceManager

/** Resource for user account administration */
@Api(value = "/admin/v1/user", listingPath = "/doc/admin/v1/user",
  description = "Allow for user account administration")
class UserResource(
    multiUserProvider: MultiAuthProvider,
    serviceManager: ServiceManager,
    store: ProfileDataStore with ClusterDataStore,
    override val maintenanceStatus: MaintenanceStatus
  ) extends Controller with JsonController with MaintenanceAwareController {

  import Scalaz._

  private type Conn = store.Conn
  private val registrationWizard = new UserRegistrationWizard(store, serviceManager)
  private val unregistrationWizard = new UserUnregistrationWizard(store, serviceManager)

  /** Register a new user account. */
  @ApiOperation(value = "Create a new user account", httpMethod = "POST",
    responseClass = "es.tid.cosmos.api.controllers.admin.RegisterUserResponse",
    notes =
      """
        Provides a mean for user registration by posting the properties of the newly
        created user.  The properties have the following restrictions:

        <ul>
          <li>authId: non-empty string that must be unique per authorization realm.</li>
          <li>authRealm: identifier of the authorization realm (also a non-empty string).</li>
          <li>email: email address to contact the user about maintenance windows or other
          conditions and announcements.</li>
          <li>handle: user handle to be used as SSH login. It must be a valid unix login
          (letters and numbers with a leading letter) and at least three characters.</li>
          If this field is not present, one will be generated.</li>
          <li>sshPublicKey: must be a public key in the same format SSH stores it
          (ssh-rsa|ssh-dsa, the key and the user email).</li>
        </ul>
      """)
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
      dbInfo <- store.withTransaction { implicit c =>
        for {
          userId <- uniqueUserId(params)
          handle <- selectHandle(params.handle)
        } yield (userId, Registration(handle, params.sshPublicKey, params.email))
      }
      (userId, registration) = dbInfo
      registrationResult <- registrationWizard.registerUser(userId, registration)
        .leftMap(message => InternalServerError(Json.toJson(message)))
      (profile, _) = registrationResult
    } yield Created(Json.toJson(RegisterUserResponse(
      handle = profile.handle,
      apiKey = profile.apiCredentials.apiKey,
      apiSecret = profile.apiCredentials.apiSecret
    )))
  }

  /** Unregister a new user account. */
  @ApiOperation(value = "Delete an existing user account", httpMethod = "DELETE",
    responseClass = "es.tid.cosmos.api.controllers.common.Message")
  @ApiErrors(Array(
    new ApiError(code = 401, reason = "Missing auth header"),
    new ApiError(code = 403, reason = "Forbidden"),
    new ApiError(code = 404, reason = "User does not exist"),
    new ApiError(code = 500, reason = "Account deletion failed"),
    new ApiError(code = 503, reason = "Service is under maintenance")
  ))
  def unregister(
      @ApiParam(value = "Authentication realm", required = true, defaultValue = "realm")
      @PathParam("realm")
      realm: String,
      @ApiParam(value = "Authentication identifier", required = true, defaultValue = "id0000")
      @PathParam("id")
      id: String) = Action { request =>
    val userId = UserId(realm, id)
    for {
      _ <- requireResourceNotUnderMaintenance()
      _ <- requireAdminCreds(realm, request.headers)
      unregistration_> <- startUnregistration(userId)
    } yield {
      val text = s"User $userId unregistration started"
      Logger.info(text)
      message(Ok, text)
    }
  }

  private def startUnregistration(userId: UserId): ActionValidation[Future[Unit]] = for {
    cosmosProfile <- store.withTransaction { implicit c =>
      store.profile.lookupByUserId(userId)
    }.toSuccess(message(NotFound, s"User $userId does not exist"))
    unregistration_> <- unregistrationWizard.unregisterUser(cosmosProfile.id)
      .leftMap(message => InternalServerError(Json.toJson(message)))
  } yield {
    unregistration_>.onSuccess {
      case _ => Logger.info(s"User $userId (${cosmosProfile.handle}) successfully unregistered")
    }
    unregistration_>.onFailure {
      case ex => Logger.error(s"Could not unregister user $userId (${cosmosProfile.handle})", ex)
    }
    unregistration_>
  }

  private def selectHandle(reqHandle: Option[String])(implicit c: Conn) =
    reqHandle match {
      case None => Success(generateHandle())
      case Some(handle) if !store.profile.handleExists(handle) => handle.success
      case Some(handle) => failWith(Conflict, s"Handle '$handle' is already taken")
    }

  private def uniqueUserId(params: RegisterUserParams)
                          (implicit c: Conn): ActionValidation[UserId] = {
    val userId = UserId(params.authRealm, params.authId)
    if (store.profile.lookupByUserId(userId).isEmpty) userId.success
    else failWith(Conflict, s"Already existing credentials: $userId")
  }

  private def requireAdminCreds(targetRealm: String, headers: Headers): ActionValidation[Unit] =
    for {
      password <- requireProviderWithExternalAdmin(targetRealm)
      authHeader <- headers.get("Authorization")
        .toSuccess(message(Unauthorized, "Missing authorization header"))
      _ <- authHeader match {
        case BasicAuth(wrongRealm, _) if targetRealm != wrongRealm => failWith(
          Forbidden, s"Trying to create users in $targetRealm with $wrongRealm credentials")
        case BasicAuth(`targetRealm`, wrongPassword) if wrongPassword != password =>
          failWith(Forbidden, "Invalid credentials")
        case BasicAuth(`targetRealm`, `password`) => ().success
        case _ => failWith(Unauthorized, "Malformed authorization header")
      }
    } yield ()

  /** Require that the provide has administration enabled.
    *
    * @param providerName  Name of the provider
    * @return              Administration password or an error
    */
  private def requireProviderWithExternalAdmin(providerName: String): ActionValidation[String] =
    for {
      provider <- multiUserProvider.providers.get(providerName)
        .toSuccess(message(NotFound, s"$providerName authentication realm does not exist"))
      password <- provider.adminPassword.toSuccess(
        message(Unauthorized, s"External user creation is disabled for the $providerName realm"))
    } yield password

  private def message(status: Status, text: String) = status(Json.toJson(Message(text)))

  private def failWith(status: Status, text: String) = message(status, text).fail

  @tailrec
  private def generateHandle()(implicit c: Conn): String = {
    val handle = s"id${Random.nextLong().abs.toString}"
    if (store.profile.handleExists(handle)) generateHandle() else handle
  }
}
