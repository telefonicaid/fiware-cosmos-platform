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

import play.api.data.validation.{Constraint, Invalid, Valid}

object HandleConstraint {

  val handleRegex = "^[a-zA-Z][a-zA-Z0-9]{2,}$".r

  val blacklist = Seq("hdfs", "hive", "hadoop", "mapred", "root")

  val constraint: Constraint[String] = Constraint[String]("constraint.handle") { handle =>
    if (blacklist.contains(handle)) Invalid(s"'$handle' is a reserved name")
    else handleRegex.unapplySeq(handle).map(_ => Valid)
      .getOrElse(Invalid("Not a valid unix handle, please use a-z letters and numbers in a non-starting position"))
  }

  def apply(input: String): Boolean = constraint(input) == Valid
}
