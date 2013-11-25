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

import es.tid.cosmos.api.controllers.pages.routes.{AdminPage, Pages}
import es.tid.cosmos.api.profile.{UserCapabilities, Capability}
import views.NavTab

object Navigation {

  /** Navigation tabs for regular users */
  val userNavigation: Seq[NavTab] = Seq(
    NavTab("profile", "Profile", Pages.showProfile().toString()),
    NavTab("getting-started", "Getting started", Pages.customGettingStarted().toString())
  )

  /** Operators are user with access to the admin page */
  val operatorNavigation: Seq[NavTab] = userNavigation :+ NavTab(
    "admin", "Admin", AdminPage.show().toString()
  )

  /** Select appropriate navigation given set of capabilities */
  def forCapabilities(capabilities: UserCapabilities): Seq[NavTab] =
    if (capabilities.hasCapability(Capability.IsOperator)) operatorNavigation
    else userNavigation
}
