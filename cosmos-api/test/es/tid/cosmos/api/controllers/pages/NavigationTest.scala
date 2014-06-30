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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile._
import views.NavTab

class NavigationTest extends FlatSpec with MustMatchers {
  
  "Untrusted user capabilities" must "have navigation links to profile and getting-started pages" in {
    tabIds(Navigation.forCapabilities(UntrustedUserCapabilities)) must be (Set(
      "profile", "getting-started", "faq"
    ))
  }

  "Operator user capabilities" must "have regular users links and the admin page link" in {
    val operatorCapabilities = UntrustedUserCapabilities + Capability.IsOperator
    tabIds(Navigation.forCapabilities(operatorCapabilities)) must be (Set(
      "profile", "getting-started", "admin", "faq"
    ))
  }

  def tabIds(tabs: Seq[NavTab]) = tabs.map(_.id).toSet
}
