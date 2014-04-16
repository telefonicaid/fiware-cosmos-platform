/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.api.controllers.pages

import es.tid.cosmos.api.controllers.pages.routes.{AdminPage, Pages}
import es.tid.cosmos.api.profile.{UserCapabilities, Capability}
import views.NavTab

/** Navigation tab utilities */
object Navigation {

  /** Navigation tabs for regular users */
  lazy val UserNavigation: Seq[NavTab] = Seq(
    NavTab("profile", "Profile", Pages.showProfile().toString()),
    NavTab("getting-started", "Getting started", Pages.customGettingStarted().toString()),
    NavTab("faq", "FAQ", Pages.faq().toString())
  )

  /** Operators are user with access to the admin page */
  lazy val OperatorNavigation: Seq[NavTab] = UserNavigation :+ NavTab(
    "admin", "Admin", AdminPage.show().toString()
  )

  /** Select appropriate navigation given set of capabilities */
  def forCapabilities(capabilities: UserCapabilities): Seq[NavTab] =
    if (capabilities.hasCapability(Capability.IsOperator)) OperatorNavigation
    else UserNavigation
}
