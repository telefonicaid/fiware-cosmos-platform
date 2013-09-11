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

package es.tid.cosmos.api.oauth2

import es.tid.cosmos.api.profile.UserId

case class UserProfile(
    id: UserId,
    name: Option[String] = None,
    email: Option[String] = None) {

  def contact: String = {
    val parts = Seq(name, email.map { value => s"($value)" }).flatten
    if (parts.isEmpty) "--" else parts.mkString(" ")
  }
}
