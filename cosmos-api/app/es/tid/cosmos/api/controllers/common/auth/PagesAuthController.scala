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

package es.tid.cosmos.api.controllers.common.auth

import scalaz._

import play.api.mvc._

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.controllers.pages.routes
import es.tid.cosmos.api.profile.{CosmosProfile, CosmosDao, UserId}
import es.tid.cosmos.api.profile.UserState._

/** Controller mixin adding authentication validations. */
trait PagesAuthController { this: Controller =>
  val dao: CosmosDao

  import Scalaz._

  /** Checks cookie and DB information to clear inconsistent cookies and
    * let you execute a different action for authenticated and unauthenticated
    * requests.
    *
    * @param request               Request we are dispatching actions
    * @param whenRegistered        Action to use when the user is registered
    * @param whenNotRegistered     Action to use when the user is authenticated but not registered
    * @param whenNotAuthenticated  Action to use when the user is not authenticated
    * @return                      Output of one or the actions or a redirection to the index
    */
  def withAuthentication(request: RequestHeader)(
      whenRegistered: (OAuthUserProfile, CosmosProfile) => SimpleResult,
      whenNotRegistered: OAuthUserProfile => SimpleResult,
      whenNotAuthenticated: => SimpleResult): SimpleResult =
    request.session.userProfile.map(userProfile =>
      dao.store.withTransaction { implicit c =>
        dao.profile.lookupByUserId(userProfile.id)
      } match {
        case Some(cosmosProfile)
          if cosmosProfile.state == Enabled || cosmosProfile.state == Creating =>
          whenRegistered(userProfile, cosmosProfile)
        case _ => whenNotRegistered(userProfile)
      }
    ).getOrElse(whenNotAuthenticated)

  /** Action validation requiring a registered user to be authenticated.
    *
    * @param request  Action request
    * @return         Either the user profiles or an error redirecting to the login page
    *                 or the registration page
    */
  def requireUserProfiles(
      request: RequestHeader): ActionValidation[(OAuthUserProfile, CosmosProfile)] =
    for {
      userProfile <- requireAuthenticatedUser(request)
      cosmosProfile <- requireRegisteredUser(userProfile.id)
    } yield (userProfile, cosmosProfile)

  /** Action validation requiring an authenticated user.
    *
    * @param request     Action request
    * @return            Either the user profile or a redirection response
    */
  def requireAuthenticatedUser(request: RequestHeader): ActionValidation[OAuthUserProfile] =
    request.session.userProfile.toSuccess(redirectToIndex)

  /** Action validation requiring a registered user.
    *
    * @param userId      Id of the user
    * @param redirectTo  Where to redirect unregistered users, the registration page by default
    * @return            Either the cosmos profile or a redirection response
    */
  def requireRegisteredUser(
      userId: UserId,
      redirectTo: Call = routes.Pages.registerForm()): ActionValidation[CosmosProfile] =
    dao.store.withTransaction { implicit c =>
      dao.profile.lookupByUserId(userId)
    }.toSuccess(Redirect(redirectTo))

  lazy val redirectToIndex = Redirect(routes.Pages.index())
  lazy val redirectToRegistration = Redirect(routes.Pages.registerForm())
}
