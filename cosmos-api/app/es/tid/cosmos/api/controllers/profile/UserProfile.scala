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

package es.tid.cosmos.api.controllers.profile

import play.api.data.validation.ValidationError
import play.api.libs.functional.syntax._
import play.api.libs.json._

import es.tid.cosmos.api.profile.{EmailConstraint, NamedKey}

/**
 * Profile as seen by an API client
 */
case class UserProfile(handle: String, email: String, keys: List[NamedKey])

object UserProfile {

  implicit val userProfileReads: Reads[UserProfile] = (
    (__ \ "handle").read[String] ~
    (__ \ "email").read[String]
        .filter(ValidationError("not a valid email"))(EmailConstraint.apply) ~
    (__ \ "keys").read[List[NamedKey]]
  )(UserProfile.apply _)

  implicit object UserProfileWrites extends Writes[UserProfile] {
    def writes(p: UserProfile) = Json.obj(
      "handle" -> p.handle,
      "email" -> p.email,
      "keys" -> (for {
        namedKey <- p.keys.sortBy(_.name)
      } yield Json.toJson(namedKey))
    )
  }
}
