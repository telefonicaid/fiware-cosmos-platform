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
