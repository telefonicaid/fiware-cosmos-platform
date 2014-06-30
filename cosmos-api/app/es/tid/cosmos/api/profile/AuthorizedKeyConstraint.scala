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

import play.api.data.validation.{Invalid, Valid, Constraint}

/** Checks that input is a valid authorized keys line for SSH. */
object AuthorizedKeyConstraint {

  import Scalaz._

  private val fieldsNumber = 2
  private val commentWordPattern = "[-_.@a-zA-Z0-9]+"

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
    keyFields <- authorizedKeyFields(line)
    (keyType, _, comment) = keyFields
    _ <- validateKeyType(keyType)
    _ <- validateComment(comment)
  } yield line

  private def uniqueLine(input: String) = input.lines.length match {
    case 1 => input.success
    case lines => s"only one line was expected but $lines were found".failure
  }

  private def authorizedKeyFields(line: String): Validation[String, (String, String, List[String])] =
    line.trim.split("\\s+").toList match {
      case keyType :: key :: comment => (keyType, key, comment).success
      case fields => s"$fieldsNumber fields were expected but ${fields.size} found".failure
    }

  private def validateKeyType(keyType: String) =
    if (validKeyTypes.contains(keyType)) keyType.success
    else s"unexpected key type: '$keyType'".failure

  private def validateComment(comment: List[String]) =
    if (comment.forall(_.matches(commentWordPattern))) ().success
    else (s"Key comment ${comment.mkString("\"", " ", "\"")} contains invalid characters. " +
         "Allowed: letters, numbers, '.', -, _ and @. Examples: 'user@host', 'Laptop key'").failure
}
