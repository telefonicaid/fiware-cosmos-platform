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

package es.tid.cosmos.api.auth.multiauth

import com.typesafe.config.Config

import es.tid.cosmos.api.auth.oauth2.OAuthProvider

case class StubProvider(name: String, config: Config) extends OAuthProvider {
  override val id = name
  override def requestUserProfile(token: String) = ???
  override def requestAccessToken(code: String, redirectUrl: String) = ???
  override def newAccountUrl = ???
  override def authenticationUrl(redirectUrl: String) = ???
}
