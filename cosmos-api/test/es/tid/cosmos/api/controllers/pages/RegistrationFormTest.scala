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

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.profile.UserId

class RegistrationFormTest extends FlatSpec with MustMatchers {

  val validInputs = Map(
    "handle" -> "jsmith",
    "publicKey" -> "ssh-rsa ADKDJDIEJDJ jsmith@example.com",
    "email" -> "jsmith@example.com"
  )

  def bindForm(inputs: Map[String, String]) = RegistrationForm().bind(inputs)

  "A registration form" must "recognise correct inputs" in {
    bindForm(validInputs).hasErrors must be (false)
  }

  it must "require a non-empty handle" in {
    bindForm(validInputs - "handle").hasErrors must be (true)
    bindForm(validInputs.updated("handle", "")).hasErrors must be (true)
  }

  it must "require three characters long handles" in {
    bindForm(validInputs.updated("handle", "ab")).hasErrors must be (true)
    bindForm(validInputs.updated("handle", "abc")).hasErrors must be (false)
  }

  it must "require handles to be a single word" in {
    bindForm(validInputs.updated("handle", "two words")).hasErrors must be (true)
  }

  it must "require handles to start by a letter" in {
    bindForm(validInputs.updated("handle", "1nvalid")).hasErrors must be (true)
  }

  it must "require a valid public key" in {
    bindForm(validInputs - "publicKey").hasErrors must be (true)
    bindForm(validInputs.updated("publicKey", "")).hasErrors must be (true)
  }

  it must "require valid email" in {
    bindForm(validInputs - "email").hasErrors must be (true)
    bindForm(validInputs.updated("email", "not an email")).hasErrors must be (true)
  }

  val userProfile = OAuthUserProfile(
    id = UserId("realm", "db1234"),
    name = Some("John Smith"),
    email = Some("jsmith@example.com")
  )

  it must "infer email and handle from OAuth profile" in {
    val form = RegistrationForm.initializeFrom(userProfile)
    form.data must contain ("email" -> "jsmith@example.com")
    form.data must contain ("handle" -> "jsmith")
  }

  it must "infer nothing when email is missing for OAuth profile" in {
    val form = RegistrationForm.initializeFrom(userProfile.copy(email = None))
    form.data.get("email") must not be 'defined
    form.data.get("handle") must not be 'defined
  }
}
