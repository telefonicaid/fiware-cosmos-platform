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
