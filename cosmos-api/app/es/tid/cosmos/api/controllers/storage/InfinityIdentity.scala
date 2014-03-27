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

package es.tid.cosmos.api.controllers.storage

import play.api.libs.json.{Json, Writes}

/** Identity of an Infinity's user.
  *
  * @param user  User handle
  * @param group  User group
  * @param accessMask  Unix-style access mask
  * @param origins  Optionally, the set of allowed origins this identity can be used
  *                 from. When missing, all origins are OK.
  */
case class InfinityIdentity(
    user: String,
    group: String,
    accessMask: AccessMask,
    origins: Option[Set[String]] = None
)

object InfinityIdentity {
  implicit val identityWrites: Writes[InfinityIdentity] = Json.writes[InfinityIdentity]
}
