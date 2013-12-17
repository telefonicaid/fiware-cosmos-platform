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
