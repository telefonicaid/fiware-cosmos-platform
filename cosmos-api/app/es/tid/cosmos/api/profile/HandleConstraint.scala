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

import play.api.data.validation.{Constraint, Invalid, Valid}

import es.tid.cosmos.api.profile.dao.{GroupDataStore, ProfileDataStore}

object HandleConstraint {

  import Scalaz._

  val handleRegex = "^[a-zA-Z][a-zA-Z0-9]{2,}$".r

  val blacklist = Seq("hdfs", "hive", "hadoop", "mapred", "root")

  val constraint: Constraint[String] = Constraint[String]("constraint.handle") { handle =>
    if (blacklist.contains(handle)) Invalid(s"'$handle' is a reserved name")
    else handleRegex.unapplySeq(handle).map(_ => Valid)
      .getOrElse(Invalid("Not a valid unix handle, please use a-z letters and numbers in a non-starting position"))
  }

  def apply(input: String): Boolean = constraint(input) == Valid

  def ensureFreeHandle(handle: String, store: ProfileDataStore with GroupDataStore): Validation[String, Unit] = {
    lazy val error = s"Handle '$handle' is already taken".failure
    store.withTransaction { implicit c =>
      val noProfileWithHandle =
        if (!store.profile.handleExists(handle)) ().success
        else error
      val noGroupWithHandle =
        if (store.group.lookupByName(handle).isEmpty) ().success
        else error
      for {
        _ <- noProfileWithHandle
        _ <- noGroupWithHandle
      } yield ()
    }
  }
}
