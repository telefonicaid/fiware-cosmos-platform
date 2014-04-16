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

package es.tid.cosmos.api.controllers.infinity

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
