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

import play.api.data.Forms
import play.api.data.validation.{ValidationResult, Invalid, Valid, Constraint}

/**
 * Checks that input is a valid authorized keys line for SSH.
 */
object AuthorizedKeyConstraint {

  val fieldsNumber = 3
  val validKeyTypes = Set("ssh-rsa", "ssh-dsa")
  val emailConstraint = Forms.email.constraints.head

  val constraint: Constraint[String] = Constraint("constraint.authorizedKey") { input =>
    havingOneLine(input) { line =>
      havingThreeFields(line) { (keyType, _, email) =>
        if (!validKeyType(keyType)) Invalid(s"unexpected key type: '$keyType'")
        else if (!validEmail(email)) Invalid(s"invalid email '$email'")
        else Valid
      }
    }
  }

  def apply(signature: String): Boolean = constraint(signature) == Valid

  private def havingOneLine(input: String)(f: String => ValidationResult) = {
    val lines = input.lines.length
    if (lines != 1)
      Invalid(s"only one line was expected but $lines were found")
    else f(input)
  }

  private def havingThreeFields(line: String)(f: (String, String, String) => ValidationResult) =
    line.trim.split("\\s+").toList match {
      case List(keyType, key, email) => f(keyType, key, email)
      case fields => Invalid(s"$fieldsNumber fields were expected but ${fields.size} were found")
    }

  private def validKeyType(keyType: String) = validKeyTypes.contains(keyType)

  private def validEmail(email: String) = emailConstraint(email) == Valid
}
