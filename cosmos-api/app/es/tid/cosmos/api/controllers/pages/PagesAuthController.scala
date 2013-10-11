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

import play.api.mvc.{RequestHeader, Result, Controller}

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.profile.{CosmosProfile, CosmosProfileDao}

trait PagesAuthController extends Controller {
  val dao: CosmosProfileDao

  /**
   * Checks cookie and DB information to clear inconsistent cookies and
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
      whenRegistered: (OAuthUserProfile, CosmosProfile) => Result,
      whenNotRegistered: OAuthUserProfile => Result,
      whenNotAuthenticated: => Result): Result =
    request.session.userProfile.map(userProfile =>
      dao.withTransaction { implicit c =>
        dao.lookupByUserId(userProfile.id)
      }.map(cosmosProfile =>
        whenRegistered(userProfile, cosmosProfile)
      ).getOrElse(whenNotRegistered(userProfile))
    ).getOrElse(whenNotAuthenticated)

  /**
   * Similar to withAuthentication with default behaviour: unregistered users
   * are redirected to the registration form and unauthenticated users to the index.
   *
   * @param request         Request to dispatch actions for
   * @param f  Action to execute when the user is registered
   * @return                Result of whenRegistered or an appropriate redirection
   */
  def whenRegistered(request: RequestHeader)(f: (OAuthUserProfile, CosmosProfile) => Result): Result =
    withAuthentication(request)(
      whenRegistered = f,
      whenNotRegistered = _ => Redirect(routes.Pages.registerForm()),
      whenNotAuthenticated = redirectToIndex
    )

  private lazy val redirectToIndex = Redirect(routes.Pages.index())
}
