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

package es.tid.cosmos.api.controllers

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.data.validation.{Invalid, Valid}

class AuthorizedKeyConstraintTest extends FlatSpec with MustMatchers {

  val validate = AuthorizedKeyConstraint.authorizedKey

  "The authorized key constraint" must "accept rsa keys" in {
    validate("ssh-rsa ADKDJDIEJDJ jsmith@example.com") must be (Valid)
  }

  it must "accept dsa keys" in {
    validate("ssh-dsa ADKDJDIEJDJ jsmith@example.com") must be (Valid)
  }

  it must "reject other kinds of keys" in {
    validate("ssh-xxx ADKDJDIEJDJ jsmith@example.com") must be (
      Invalid("unexpected key type: 'ssh-xxx'"))
  }

  it must "accept keys with extra spaces" in {
    validate("  ssh-rsa  ADKDJDIEJDJ   jsmith@example.com   ") must be (Valid)
  }

  it must "reject lines with missing fields" in {
    validate("ssh-rsa ADKDJDIEJDJ") must equal (
      Invalid("3 fields were expected but 2 were found"))
  }

  it must "rejest multi-line inputs" in {
    validate("""ssh-rsa ADKDJDIEJDJ jsmith@example.com
               |ssh-rsa ADKDJDIEJDJ jsmith@example.com""".stripMargin) must be(
      Invalid("only one line was expected but 2 were found")
    )
  }

  it must "reject keys with invalid email addresses" in {
    validate("ssh-rsa ADKDJDIEJDJ adamos@tid@e.es") must be(
      Invalid("invalid email 'adamos@tid@e.es'"))
  }

  it must "be available as a simple function" in {
    AuthorizedKeyConstraint.validate("ssh-rsa ADKDJDIEJDJ jsmith@example.com") must be (true)
  }
}
