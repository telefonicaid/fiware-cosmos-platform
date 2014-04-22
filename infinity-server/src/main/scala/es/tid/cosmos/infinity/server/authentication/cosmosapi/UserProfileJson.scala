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

package es.tid.cosmos.infinity.server.authentication.cosmosapi

import es.tid.cosmos.infinity.server.permissions.PermissionsMask
import es.tid.cosmos.infinity.server.authentication.UserProfile

private[cosmosapi] case class UserProfileJson(
    user: String,
    group: String,
    accessMask: String,
    origins: Option[List[String]]) {

  require(PermissionsMask.isValidOctal(accessMask),
    s"invalid access mask expression '$accessMask': octal value was expected")

  def toUserProfile(superGroup: String) = UserProfile(
    username = user,
    group = group,
    mask = PermissionsMask.fromOctal(accessMask),
    accessFrom = origins.map(_.toSet),
    superuser = group == superGroup
  )
}
