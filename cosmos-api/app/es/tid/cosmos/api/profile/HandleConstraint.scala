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

package es.tid.cosmos.api.profile

import play.api.data.validation.Constraints._
import play.api.data.validation.{Valid, Constraint}

object HandleConstraint {

  val handleRegex = "^[a-zA-Z][a-zA-Z0-9]{2,}$".r

  val constraint: Constraint[String] = pattern(handleRegex, error = "Not a valid unix handle, " +
    "please use a-z letters and numbers in a non-starting position")

  def apply(input: String): Boolean = constraint(input) == Valid
}
