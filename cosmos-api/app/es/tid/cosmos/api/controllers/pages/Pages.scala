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

package es.tid.cosmos.api.controllers.pages

import scala.concurrent.Future
import scalaz._

import com.typesafe.config.Config
import dispatch.{Future => _, _}, Defaults._
import play.Logger
import play.api.data.Form
import play.api.libs.json.Json
import play.api.mvc.{Controller, RequestHeader, Action}

import _root_.controllers.{routes => rootRoutes}
import es.tid.cosmos.api.auth.oauth2.OAuthError.UnauthorizedClient
import es.tid.cosmos.api.auth.oauth2._
import es.tid.cosmos.api.auth.multiauth.MultiAuthProvider
import es.tid.cosmos.api.controllers.admin.MaintenanceStatus
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.common.auth.PagesAuthController
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.profile.{CosmosProfile, Registration, UserId}
import es.tid.cosmos.api.profile.dao.CosmosDao
import es.tid.cosmos.api.task.TaskDao
import es.tid.cosmos.api.wizards.UserRegistrationWizard
import es.tid.cosmos.common.Wrapped
import es.tid.cosmos.servicemanager.ServiceManager
import views.AuthAlternative

/** Controller for the web pages of the service. */
class Pages(
    multiAuthProvider: MultiAuthProvider,
    serviceManager: ServiceManager,
    override val taskDao: TaskDao,
    override val dao: CosmosDao,
    override val maintenanceStatus: MaintenanceStatus,
    config: Config
  ) extends Controller with JsonController with PagesAuthController
    with MaintenanceAwareController with TaskController {

  import Scalaz._

  private val registrationWizard = new UserRegistrationWizard(serviceManager)

  def index = Action { implicit request =>
    withAuthentication(request)(
      whenRegistered = (_, _) => Redirect(routes.Pages.showProfile()),
      whenNotRegistered = _ => Redirect(routes.Pages.registerForm()),
      whenNotAuthenticated = landingPage
    )
  }

  def swaggerUI = Action { implicit request =>
    Ok(views.html.swaggerUI(AbsoluteUrl(rootRoutes.ApiHelpController.getResources())))
  }

  def authorize(providerId: String, maybeCode: Option[String], maybeError: Option[String]) =
    Action.async { implicit request =>

      def authorizeCode(oauthClient: OAuthProvider, code: String) =
        (for {
          token <- oauthClient.requestAccessToken(code, redirectUrl(oauthClient.id))
          userProfile <- oauthClient.requestUserProfile(token)
        } yield {
          Logger.info(s"${userProfile.id} authorized with token $token")
          Redirect(routes.Pages.showProfile())
            .withSession(session.setToken(token).setUserProfile(userProfile))
        }) recover {
          case ex @ OAuthException(_, _) => unauthorizedPage(ex)
          case Wrapped(Wrapped(ex: OAuthException)) => unauthorizedPage(ex)
        }

      def reportMissingAuthCode = BadRequest(Json.toJson(ErrorMessage("Missing code")))

      for {
        oauthClient <- oauthProviderById(providerId)
        _ <- requireNoOAuthError(maybeError)
      } yield (maybeCode, maybeError.flatMap(OAuthError.parse)) match {
        case (Some(code), _) => authorizeCode(oauthClient, code)
        case _ => Future.successful(reportMissingAuthCode)
      }
    }

  private def requireNoOAuthError(maybeError: Option[String])
                                 (implicit request: RequestHeader): ActionValidation[Unit] = {
    val errorResponse = for {
      error <- maybeError
      oauthError <- OAuthError.parse(error)
    } yield unauthorizedPage(OAuthException(oauthError,
        "OAuth provider redirected with an error code instead of an authorization code"))
    errorResponse.toFailure(())
  }

  private def oauthProviderById(providerId: String): ActionValidation[OAuthProvider] =
    multiAuthProvider.oauthProviders.get(providerId).toSuccess(
      NotFound(Json.toJson(ErrorMessage(s"Unknown authorization provider $providerId")))
    )

  def registerForm = Action { implicit request =>
    for {
      _ <- requirePageNotUnderMaintenance()
      userProfile <- requireAuthenticatedUser(request)
      cosmosProfile <- requireUnregisteredUser(userProfile.id)
    } yield Ok(views.html.registration(userProfile, RegistrationForm.initializeFrom(userProfile)))
  }

  def registerUser = Action { implicit request =>
    for {
      _ <- requireResourceNotUnderMaintenance()
      userProfile <- requireAuthenticatedUser(request)
      _ <- requireUnregisteredUser(userProfile.id)
      validatedForm = dao.store.withTransaction { implicit c =>
        val form = RegistrationForm().bindFromRequest()
        form.data.get("handle") match {
          case Some(handle) if dao.profile.handleExists(handle) =>
            form.withError("handle", s"'$handle' is already taken")
          case _ => form
        }
      }
      registration <- requireValidRegistration(userProfile, validatedForm)
      _ <- requireNoActiveTask(registration.handle, "registration")
      wizardResult <- dao.store.withTransaction { implicit c =>
        registrationWizard.registerUser(dao, userProfile.id, registration)
          .leftMap(message => InternalServerError(Json.toJson(message)))
      }
    } yield {
      val (_, registration_>) = wizardResult
      val task = taskDao.registerTask()
        .linkToFuture(registration_>, s"Failed to register user with handle ${registration.handle}")
      task.resource = registration.handle
      task.metadata = "registration"
      redirectToIndex
    }
  }

  private def requireValidRegistration(
      userProfile: OAuthUserProfile, form: Form[Registration]): ActionValidation[Registration] =
    form.fold(
      formWithErrors => registrationPage(userProfile, formWithErrors).failure,
      registration => registration.success
    )

  private def requireUnregisteredUser(userId: UserId): ActionValidation[Unit] = {
    val userExists = dao.store.withTransaction { implicit c =>
      dao.profile.lookupByUserId(userId).isDefined
    }
    if (userExists) redirectToIndex.failure else ().success
  }

  private def registrationPage(profile: OAuthUserProfile, form: Form[Registration]) = {
    val contents = views.html.registration(profile, form)
    if (form.hasErrors) BadRequest(contents)
    else Ok(contents)
  }

  def logout() = Action { request =>
    redirectToIndex.withNewSession
  }

  private def landingPage(implicit request: RequestHeader) =
    Ok(views.html.landingPage(authAlternatives))

  def showProfile = Action { implicit request =>
    for {
      profiles <- requireUserProfiles(request)
      (userProfile, cosmosProfile) = profiles
    } yield Ok(views.html.profile(
      oauthProfile = userProfile,
      cosmosProfile = cosmosProfile,
      tabs = Navigation.forCapabilities(cosmosProfile.capabilities)
    ))
  }

  def customGettingStarted = Action { implicit request =>
    for {
      profiles <- requireUserProfiles(request)
      (_, cosmosProfile) = profiles
    } yield Ok(views.html.gettingStarted(
      cosmosProfile, Navigation.forCapabilities(cosmosProfile.capabilities)
    ))
  }

  def faq = Action { implicit request =>
    val faqConfig = config.getConfig("faq")
    if (requireAuthenticatedUser(request).isFailure)
      Ok(views.html.faq(None, faqConfig))
    else for {
      profiles <- requireUserProfiles(request)
      (_, cosmosProfile) = profiles
    } yield {
      Ok(views.html.faq(Some(Navigation.forCapabilities(cosmosProfile.capabilities)), faqConfig))
    }
  }

  private def unauthorizedPage(ex: OAuthException)(implicit request: RequestHeader) = {
    Logger.error(s"OAuth request failed: ${ex.description}", ex)
    val message = if (ex.error == UnauthorizedClient) "Access denied" else "Authorization failed"
    val body = views.html.landingPage(authAlternatives, Some(message))
    Unauthorized(body).withNewSession
  }

  private def authAlternatives(implicit request: RequestHeader): Seq[AuthAlternative] = (for {
    (id, oauthClient) <- multiAuthProvider.oauthProviders
  } yield AuthAlternative(
    id = oauthClient.id,
    name = oauthClient.name,
    authUrl = oauthClient.authenticationUrl(redirectUrl(id)),
    newAccountUrl = oauthClient.newAccountUrl
  )).toSeq

  private def redirectUrl(id: String)(implicit request: RequestHeader): String =
    AbsoluteUrl(routes.Pages.authorize(providerId = id, code = None, error = None))
}
