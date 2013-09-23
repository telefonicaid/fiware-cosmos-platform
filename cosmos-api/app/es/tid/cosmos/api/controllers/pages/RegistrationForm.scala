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

import play.api.data.Forms._
import play.api.data.Form
import play.api.data.validation.Constraints._

import es.tid.cosmos.api.controllers.AuthorizedKeyConstraint

object RegistrationForm {

  private val identifierRegex = "^[a-zA-Z][a-zA-Z0-9]*$".r

  def apply(): Form[Registration] = Form(mapping(
    "handle" -> text.verifying(minLength(3), pattern(identifierRegex,
      error="Not a valid unix handle, please use a-z letters and numbers " +
        "in a non-starting position")),
    "publicKey" -> text.verifying(AuthorizedKeyConstraint.authorizedKey)
  )(Registration.apply)(Registration.unapply))
}
