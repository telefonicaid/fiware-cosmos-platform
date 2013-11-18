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
