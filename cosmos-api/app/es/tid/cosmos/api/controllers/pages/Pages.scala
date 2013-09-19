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

import dispatch.{Future => _, _}, Defaults._
import play.Logger
import play.api.data.Form
import play.api.data.Forms._
import play.api.data.validation.Constraints._
import play.api.libs.json.Json
import play.api.mvc.{RequestHeader, Action, Controller}

import _root_.controllers.{routes => rootRoutes}
import es.tid.cosmos.api.controllers._
import es.tid.cosmos.api.controllers.common.{AbsoluteUrl, ErrorMessage}
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.oauth2.OAuthError.UnauthorizedClient
import es.tid.cosmos.api.oauth2.{UserProfile, OAuthProvider, MultiOAuthProvider, OAuthError, OAuthException}
import es.tid.cosmos.api.profile.CosmosProfileDao
import es.tid.cosmos.platform.common.Wrapped
import es.tid.cosmos.servicemanager.ServiceManager
import views.AuthAlternative

/**
 * Controller for the web pages of the service.
 */
class Pages(
    oauthClients: MultiOAuthProvider,
    serviceManager: ServiceManager,
    dao: CosmosProfileDao
  ) extends Controller {

  def index = Action { implicit request =>
    if (!session.isAuthenticated) landingPage
    else {
      val maybeCosmosId = dao.withTransaction { implicit c =>
        for {
          UserProfile(id, _, _) <- session.userProfile
          cosmosProfile <- dao.lookupByUserId(id)
        } yield cosmosProfile.id
      }
      (session.isRegistered, maybeCosmosId) match {
        case (false, None) => registrationPage(session.userProfile.get, RegistrationForm())
        case (false, Some(_)) => redirectToIndex.withSession(session.setCosmosId(maybeCosmosId))
        case (true, Some(_)) if maybeCosmosId == session.cosmosId => userProfile
        case _ => redirectToIndex.withNewSession
      }
    }
  }

  def swaggerUI = Action { implicit request =>
    Ok(views.html.swaggerUI(AbsoluteUrl(rootRoutes.ApiHelpController.getResources())))
  }

  def authorize(providerId: String, maybeCode: Option[String], maybeError: Option[String]) =
    Action { implicit request =>

      def authorizeCode(oauthClient: OAuthProvider, code: String) = Async {
        (for {
          token <- oauthClient.requestAccessToken(code)
          userProfile <- oauthClient.requestUserProfile(token)
          maybeCosmosId = dao.withConnection {
            implicit c => dao.getCosmosId(userProfile.id)
          }
        } yield {
          Logger.info(s"Authorized with token $token")
          Redirect(routes.Pages.index()).withSession(session
            .setToken(token)
            .setUserProfile(userProfile)
            .setCosmosId(maybeCosmosId))
        }) recover {
          case ex@OAuthException(_, _) => unauthorizedPage(ex)
          case Wrapped(Wrapped(ex: OAuthException)) => unauthorizedPage(ex)
        }
      }

      def reportAuthError(error: OAuthError.OAuthError) = unauthorizedPage(OAuthException(error,
        "OAuth provider redirected with an error code instead of an authorization code"))

      def reportMissingAuthCode = BadRequest(Json.toJson(ErrorMessage("Missing code")))

      def reportUnknownProvider = NotFound(Json.toJson(
        ErrorMessage(s"Unknown authorization provider $providerId")))

      oauthClients.providers.get(providerId).map(oauthClient =>
        (maybeCode, maybeError.flatMap(OAuthError.parse)) match {
          case (_, Some(error)) => reportAuthError(error)
          case (Some(code), _) => authorizeCode(oauthClient, code)
          case _ => reportMissingAuthCode
        }
      ).getOrElse(reportUnknownProvider)
    }

  def registerUser = Action { implicit request =>
    session.userProfile.map(userProfile =>
      dao.withTransaction { implicit c =>

        def createCosmosProfile(reg: Registration) = {
          dao.registerUserInDatabase(userProfile.id, reg)
          val cosmosProfile = dao.lookupByUserId(userProfile.id)
            .getOrElse(throw new IllegalStateException(
              "Could not read registration information from database"))
          serviceManager.addUsers(serviceManager.persistentHdfsId, cosmosProfile.toClusterUser)
          cosmosProfile
        }

        val validatedForm = {
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
            val cosmosProfile = createCosmosProfile(registration)
            redirectToIndex.withSession(session.setCosmosId(cosmosProfile.id))
          }
        )
      }).getOrElse(Forbidden(Json.toJson(ErrorMessage("Not authenticated"))))
  }

  def logout() = Action { request =>
    redirectToIndex.withNewSession
  }

  private def redirectToIndex = Redirect(routes.Pages.index())

  private def landingPage(implicit request: RequestHeader) =
    Ok(views.html.landingPage(authAlternatives))

  private def registrationPage(profile: UserProfile, form: Form[Registration]) = {
    val contents = views.html.registration(profile, form)
    if (form.hasErrors) BadRequest(contents)
    else Ok(contents)
  }

  private def userProfile(implicit request: RequestHeader) = {
    dao.withConnection { implicit c =>
      val userProfile = session.userProfile.get
      Ok(views.html.profile(userProfile, dao.lookupByUserId(userProfile.id).get))
    }
  }

  private def unauthorizedPage(ex: OAuthException)(implicit request: RequestHeader) = {
    Logger.error(s"OAuth request failed: ${ex.description}", ex)
    val message = if (ex.error == UnauthorizedClient) "Access denied" else "Authorization failed"
    val body = views.html.landingPage(authAlternatives, Some(message))
    Unauthorized(body).withNewSession
  }

  private def authAlternatives(implicit request: RequestHeader): Seq[AuthAlternative] = (for {
    (id, oauthClient) <- oauthClients.providers
    redirectUrl = AbsoluteUrl(routes.Pages.authorize(providerId = id, code = None, error = None))
  } yield AuthAlternative(
    id = oauthClient.id,
    name = oauthClient.name,
    authUrl = oauthClient.authenticationUrl(redirectUrl),
    newAccountUrl = oauthClient.newAccountUrl
  )).toSeq
}
