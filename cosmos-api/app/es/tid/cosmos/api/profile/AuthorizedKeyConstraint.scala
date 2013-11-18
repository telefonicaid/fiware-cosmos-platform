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

import play.api.data.validation.{Invalid, Valid, Constraint}

/**
 * Checks that input is a valid authorized keys line for SSH.
 */
object AuthorizedKeyConstraint {

  import Scalaz._

  val fieldsNumber = 3
  val validKeyTypes = Set("ssh-rsa", "ssh-dsa")
  val constraint: Constraint[String] = Constraint("constraint.authorizedKey") { input =>
    validateAuthorizedKey(input).fold(
      fail = errorMessage => Invalid(errorMessage),
      succ = _ => Valid
    )
  }

  def apply(signature: String): Boolean = constraint(signature) == Valid

  private def validateAuthorizedKey(input: String) = for {
    line <- uniqueLine(input)
    (keyType, _, email) <- authorizedKeyFields(line)
    _ <- validateKeyType(keyType)
    _ <- EmailConstraint.validate(email)
  } yield line

  private def uniqueLine(input: String) = input.lines.length match {
    case 1 => input.success
    case lines => s"only one line was expected but $lines were found".failure
  }

  private def authorizedKeyFields(line: String) =
    line.trim.split("\\s+").toList match {
      case List(keyType, key, email) => (keyType, key, email).success
      case fields => s"$fieldsNumber fields were expected but ${fields.size} were found".failure
    }

  private def validateKeyType(keyType: String) =
    if (validKeyTypes.contains(keyType)) keyType.success
    else s"unexpected key type: '$keyType'".failure
}
