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

import es.tid.cosmos.api.profile.{AuthorizedKeyConstraint, HandleConstraint, Registration}

object RegistrationForm {

  def apply(): Form[Registration] = Form(mapping(
    "handle" -> text.verifying(HandleConstraint.constraint),
    "publicKey" -> text.verifying(AuthorizedKeyConstraint.constraint)
  )(Registration.apply)(Registration.unapply))
}
