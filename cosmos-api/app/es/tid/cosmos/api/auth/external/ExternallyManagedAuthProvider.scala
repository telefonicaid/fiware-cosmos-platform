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

package es.tid.cosmos.api.auth.external

import com.typesafe.config.{ConfigException, Config}

import es.tid.cosmos.api.auth.AuthProvider

/** An auth provider that is managed by an external component.
  *
  * This authentication provider is aimed to manage user accounts that are owned by a
  * component external to Cosmos. Such a component can use the admin API in order to add
  * or remove users from the realm represented by this provider.
  */
class ExternallyManagedAuthProvider(override val id: String, config: Config) extends AuthProvider {

  override val adminPassword = try {
    Some(config.getString("password"))
  } catch {
    case ex: ConfigException.Missing =>
      throw new IllegalArgumentException(
        s"Missing password configuration for the $id externally managed auth provider")
  }
}
