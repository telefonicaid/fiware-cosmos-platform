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

package es.tid.cosmos.api.auth.horizon

import com.typesafe.config.{ConfigException, Config}

import es.tid.cosmos.api.auth.{AdminEnabledAuthProvider, AuthProvider}

class HorizonAuthProvider(override val id: String, config: Config)
  extends AuthProvider with AdminEnabledAuthProvider {

  override val adminPassword = try {
    config.getString("password")
  } catch {
    case ex: ConfigException.Missing =>
      throw new IllegalArgumentException(
        s"Missing password configuration for the $id auth provider")
  }
}
