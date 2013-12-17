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

package es.tid.cosmos.api.auth.oauth2

import es.tid.cosmos.api.profile.UserId

case class OAuthUserProfile(
    id: UserId,
    name: Option[String] = None,
    email: Option[String] = None) {

  def contact: String =
    name orElse email.map(parenthesize) getOrElse parenthesize(id.toString)

  private def parenthesize(text: String) = s"($text)"
}
