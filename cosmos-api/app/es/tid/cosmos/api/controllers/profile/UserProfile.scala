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

import play.api.libs.json._

import es.tid.cosmos.api.controllers.pages.NamedKey

/**
 * Profile as seen by an API client
 */
case class UserProfile(handle: String, registeredKeys: List[NamedKey])

object UserProfile {

  implicit object UserProfileWrites extends Writes[UserProfile] {
    def writes(p: UserProfile) = Json.obj(
      "handle" -> p.handle,
      "keys" -> p.registeredKeys.sortBy(_.name)
    )
  }

  private implicit object NamedKeyWrites extends Writes[NamedKey] {
    def writes(k: NamedKey): JsValue = Json.obj(
      "name" -> k.name,
      "signature" -> k.signature
    )
  }
}
