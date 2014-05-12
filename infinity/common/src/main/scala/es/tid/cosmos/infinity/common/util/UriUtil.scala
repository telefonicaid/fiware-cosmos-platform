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

package es.tid.cosmos.infinity.common.util

import java.net.URI

object UriUtil {

  /** Replace URI scheme keeping all other fields */
  def replaceScheme(uri: URI, newScheme: String): URI =
    new URI(newScheme, uri.getUserInfo, uri.getHost, uri.getPort, uri.getPath, uri.getRawQuery,
      uri.getFragment)

  def replaceAuthority(uri: URI, newAuthority: String): URI =
    new URI(uri.getScheme, newAuthority, uri.getPath, uri.getRawQuery, uri.getFragment)

  def replacePath(uri: URI, newPath: String): URI =
    new URI(uri.getScheme, uri.getAuthority, newPath, uri.getRawQuery, uri.getFragment)
}
