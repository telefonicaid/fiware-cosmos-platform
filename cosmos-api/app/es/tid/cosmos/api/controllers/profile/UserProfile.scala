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

package es.tid.cosmos.api.controllers.profile

import play.api.libs.functional.syntax._
import play.api.libs.json._

import es.tid.cosmos.api.profile.NamedKey

/**
 * Profile as seen by an API client
 */
case class UserProfile(handle: String, keys: List[NamedKey])

object UserProfile {

  implicit val userProfileReads: Reads[UserProfile] = (
    (__ \ "handle").read[String] ~
    (__ \ "keys").read[List[NamedKey]]
  )(UserProfile.apply _)

  implicit object UserProfileWrites extends Writes[UserProfile] {
    def writes(p: UserProfile) = Json.obj(
      "handle" -> p.handle,
      "keys" -> (for {
        namedKey <- p.keys.sortBy(_.name)
      } yield Json.toJson(namedKey))
    )
  }
}
