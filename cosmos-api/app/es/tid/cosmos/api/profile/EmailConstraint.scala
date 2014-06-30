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

package es.tid.cosmos.api.profile

import scalaz._

import play.api.data.validation._
import play.api.data.validation.ValidationError

/** Email checking utilities */
object EmailConstraint {

  import Scalaz._

  /** Email form constraint */
  val value: Constraint[String] = Constraints.pattern(
      """\b[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9-]+(?:\.[a-zA-Z0-9-]+)*\b""".r,
      "constraint.email",
      "invalid email")

  /** Validation of an email
    *
    * @param email  Email to validate
    * @return       Valid email or an error message
    */
  def validate(email: String): Validation[String, String] = EmailConstraint.value(email) match {
    case Valid => email.success
    case Invalid(errors) => formatErrors(errors).failure
  }

  /** Boolean validation of an email
    *
    * @param email Email to validate
    * @return      True or false
    */
  def apply(email: String): Boolean = validate(email).isSuccess

  private def formatErrors(errors: Seq[ValidationError]) = (for {
    ValidationError(formatString, args) <- errors
  } yield formatString.format(args)).mkString(", ")
}
