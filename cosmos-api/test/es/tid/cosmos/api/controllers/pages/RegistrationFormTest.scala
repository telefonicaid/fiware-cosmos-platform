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
