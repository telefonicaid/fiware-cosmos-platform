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
import play.api.data.{Forms, Form}

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.profile.{AuthorizedKeyConstraint, HandleConstraint, Registration}

object RegistrationForm {

  def apply(): Form[Registration] = Form(mapping(
    "handle" -> text.verifying(HandleConstraint.constraint),
    "publicKey" -> text.verifying(AuthorizedKeyConstraint.constraint),
    "email" -> Forms.email
  )(Registration.apply)(Registration.unapply))

  /** Take advantage of OAuth profile information to infer form default values. */
  def initializeFrom(userProfile: OAuthUserProfile): Form[Registration] = {
    val inferredFields = (for {
      email <- userProfile.email
    } yield Map (
      "email" -> email,
      "handle" -> email.takeWhile(_.isLetterOrDigit)
    )).getOrElse(Map.empty)
    RegistrationForm().bind(inferredFields)
  }
}
