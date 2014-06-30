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

package es.tid.cosmos.api.controllers

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.data.validation.{Invalid, Valid}
import es.tid.cosmos.api.profile.AuthorizedKeyConstraint

class AuthorizedKeyConstraintTest extends FlatSpec with MustMatchers {

  val validate = AuthorizedKeyConstraint.constraint

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
    validate("ssh-rsa") must equal (Invalid("2 fields were expected but 1 found"))
  }

  it must "reject multi-line inputs" in {
    validate("""ssh-rsa ADKDJDIEJDJ jsmith@example.com
               |ssh-rsa ADKDJDIEJDJ jsmith@example.com""".stripMargin) must be(
      Invalid("only one line was expected but 2 were found")
    )
  }

  it must "accept keys with comments other than emails" in {
    validate("ssh-rsa ADKDJDIEJDJ Laptop key") must be (Valid)
  }

  it must "reject keys with special characters on their comment" in {
    validate("ssh-rsa ADKDJDIEJDJ |%;$0").toString must include("invalid characters")
  }

  it must "be available as a simple function" in {
    AuthorizedKeyConstraint("ssh-rsa ADKDJDIEJDJ jsmith@example.com") must be (true)
  }
}
