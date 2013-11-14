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

import dispatch.{Future => _, _}, Defaults._
import play.Logger
import play.api.data.Form
import play.api.libs.json.Json
import play.api.mvc.{RequestHeader, Action}

import _root_.controllers.{routes => rootRoutes}
import es.tid.cosmos.api.auth.{OAuthProvider, MultiAuthProvider}
import es.tid.cosmos.api.auth.oauth2.OAuthError.UnauthorizedClient
import es.tid.cosmos.api.auth.oauth2._
import es.tid.cosmos.api.controllers.admin.MaintenanceStatus
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.profile.{CosmosProfileDao, Registration}
import es.tid.cosmos.platform.common.Wrapped
import es.tid.cosmos.servicemanager.ServiceManager
import views.AuthAlternative

/**
 * Controller for the web pages of the service.
 */
class Pages(
    multiAuthProvider: MultiAuthProvider,
    serviceManager: ServiceManager,
    val dao: CosmosProfileDao,
    override val maintenanceStatus: MaintenanceStatus
  ) extends JsonController with PagesAuthController with MaintenanceAwareController {

  private val registrationWizard = new UserRegistrationWizard(dao, serviceManager)

  def index = Action { implicit request =>
    unlessPageUnderMaintenance {
      withAuthentication(request)(
        whenRegistered = (_, _) => Redirect(routes.Pages.showProfile()),
        whenNotRegistered = _ => Redirect(routes.Pages.registerForm()),
        whenNotAuthenticated = landingPage
      )
    }
  }

  def swaggerUI = Action { implicit request =>
    Ok(views.html.swaggerUI(AbsoluteUrl(rootRoutes.ApiHelpController.getResources())))
  }

  def authorize(providerId: String, maybeCode: Option[String], maybeError: Option[String]) =
    Action.async { implicit request =>

      def authorizeCode(oauthClient: OAuthProvider, code: String) =
        (for {
          token <- oauthClient.requestAccessToken(code)
          userProfile <- oauthClient.requestUserProfile(token)
        } yield {
          Logger.info(s"${userProfile.id} authorized with token $token")
          Redirect(routes.Pages.showProfile())
            .withSession(session.setToken(token).setUserProfile(userProfile))
        }) recover {
          case ex@OAuthException(_, _) => unauthorizedPage(ex)
          case Wrapped(Wrapped(ex: OAuthException)) => unauthorizedPage(ex)
        }

      def reportAuthError(error: OAuthError.OAuthError) = unauthorizedPage(OAuthException(error,
        "OAuth provider redirected with an error code instead of an authorization code"))

      def reportMissingAuthCode = BadRequest(Json.toJson(ErrorMessage("Missing code")))

      def reportUnknownProvider = NotFound(Json.toJson(
        ErrorMessage(s"Unknown authorization provider $providerId")))

      unlessPageUnderMaintenance {
        multiAuthProvider.oauthProviders.get(providerId).map(oauthClient =>
          (maybeCode, maybeError.flatMap(OAuthError.parse)) match {
            case (_, Some(error)) => Future.successful(reportAuthError(error))
            case (Some(code), _) => authorizeCode(oauthClient, code)
            case _ => Future.successful(reportMissingAuthCode)
          }
        ).getOrElse(Future.successful(reportUnknownProvider))
      }
    }

  def registerForm = Action { implicit request =>
    unlessPageUnderMaintenance {
      withAuthentication(request)(
        whenRegistered = (_, _) => redirectToIndex,
        whenNotRegistered = userProfile =>
          Ok(views.html.registration(userProfile, RegistrationForm.initializeFrom(userProfile))),
        whenNotAuthenticated = redirectToIndex
      )
    }
  }

  def registerUser = Action { implicit request =>
    unlessResourceUnderMaintenance {
      withAuthentication(request)(
        whenRegistered = (_, _) => redirectToIndex,
        whenNotAuthenticated = redirectToIndex,
        whenNotRegistered = userProfile => {

          val validatedForm = dao.withTransaction { implicit c =>
            val form = RegistrationForm().bindFromRequest()
            form.data.get("handle") match {
              case Some(handle) if dao.handleExists(handle) =>
                form.withError("handle", s"'$handle' is already taken")
              case _ => form
            }
          }

          validatedForm.fold(
            formWithErrors => registrationPage(userProfile, formWithErrors),
            registration => {
              registrationWizard.registerUser(userProfile.id, registration)
              redirectToIndex
            }
          )
        }
      )
    }
  }

  private def registrationPage(profile: OAuthUserProfile, form: Form[Registration]) = {
    val contents = views.html.registration(profile, form)
    if (form.hasErrors) BadRequest(contents)
    else Ok(contents)
  }

  def logout() = Action { request =>
    redirectToIndex.withNewSession
  }

  private def redirectToIndex = Redirect(routes.Pages.index())

  private def landingPage(implicit request: RequestHeader) =
    Ok(views.html.landingPage(authAlternatives))

  def showProfile = Action { implicit request =>
    unlessPageUnderMaintenance {
      whenRegistered(request) { (userProfile, _) =>
        dao.withTransaction { implicit c =>
          Ok(views.html.profile(userProfile, dao.lookupByUserId(userProfile.id).get))
        }
      }
    }
  }

  def customGettingStarted = Action { implicit request =>
    unlessPageUnderMaintenance {
      whenRegistered(request) { (_, cosmosProfile) =>
        Ok(views.html.gettingStarted(cosmosProfile))
      }
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
    redirectUrl = AbsoluteUrl(routes.Pages.authorize(providerId = id, code = None, error = None))
  } yield AuthAlternative(
    id = oauthClient.id,
    name = oauthClient.name,
    authUrl = oauthClient.authenticationUrl(redirectUrl),
    newAccountUrl = oauthClient.newAccountUrl
  )).toSeq
}
