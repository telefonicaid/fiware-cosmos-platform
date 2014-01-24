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

package es.tid.cosmos.platform.infinity

import java.net.URI

object UriUtil {

  /** Replace URI scheme keeping all other fields */
  def replaceScheme(uri: URI, newScheme: String): URI =
    new URI(newScheme, uri.getUserInfo, uri.getHost, uri.getPort, uri.getPath, uri.getRawQuery,
      uri.getFragment)

  def replaceAuthority(uri: URI, newAuthority: String): URI =
    new URI(uri.getScheme, newAuthority, uri.getPath, uri.getRawQuery, uri.getFragment)
}
