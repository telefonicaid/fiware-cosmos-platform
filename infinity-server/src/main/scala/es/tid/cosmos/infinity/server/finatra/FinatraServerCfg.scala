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
package es.tid.cosmos.infinity.server.finatra

import com.twitter.util.StorageUnit
import com.twitter.conversions.storage._

/**
 * @param http interface:port to bind to.
 * @param https interface:port to bind to.
 * @param admin interface:port to bind to.
 * @param certificatePath path to certificate on filesystem.
 * @param keyPath path to key on filesystem.
 * @param maxRequestSize max request size accepted.
 */
case class FinatraServerCfg(
  http:            Option[String] = None,
  https:           Option[String] = None,
  admin:           Option[String] = None,
  certificatePath: Option[String] = None,
  keyPath:         Option[String] = None,
  maxRequestSize:  StorageUnit    = 5 megabytes
  ) {
  if (https.isDefined) {
    require(certificatePath.isDefined, "CertificatePath must be defined for https")
    require(keyPath.isDefined, "keyPath must be defined for https")
  }
  require(http.isDefined || https.isDefined, "At least one endpoint (http or https) should be defined")
}
