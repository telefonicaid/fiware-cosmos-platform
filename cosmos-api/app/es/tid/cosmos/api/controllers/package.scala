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

package es.tid.cosmos.api

import scala.language.implicitConversions

import es.tid.cosmos.api.controllers.profile.UserProfile
import es.tid.cosmos.api.profile.CosmosProfile

package object controllers {

  class ClusterProfileExtensions(profile: CosmosProfile) {
    val toUserProfile = UserProfile(
      handle = profile.handle,
      email = profile.email,
      keys = profile.keys.toList
    )
  }

  implicit def extendMyProfile(profile: CosmosProfile) = new ClusterProfileExtensions(profile)
}
