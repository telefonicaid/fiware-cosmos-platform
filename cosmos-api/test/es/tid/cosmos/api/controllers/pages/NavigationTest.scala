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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile._
import views.NavTab

class NavigationTest extends FlatSpec with MustMatchers {
  
  "Untrusted user capabilities" must "have navigation links to profile and getting-started pages" in {
    tabIds(Navigation.forCapabilities(UntrustedUserCapabilities)) must be (Set(
      "profile", "getting-started"
    ))
  }

  "Operator user capabilities" must "have regular users links and the admin page link" in {
    val operatorCapabilities = UntrustedUserCapabilities + Capability.IsOperator
    tabIds(Navigation.forCapabilities(operatorCapabilities)) must be (Set(
      "profile", "getting-started", "admin"
    ))
  }

  def tabIds(tabs: Seq[NavTab]) = tabs.map(_.id).toSet
}
