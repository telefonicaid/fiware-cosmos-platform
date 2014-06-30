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
