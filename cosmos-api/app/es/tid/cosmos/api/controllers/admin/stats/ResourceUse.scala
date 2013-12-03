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

package es.tid.cosmos.api.controllers.admin.stats

import play.api.libs.json.{Json, Writes}

/** Current use of a resource
  *
  * @param total      Total resource units
  * @param available  Unused units
  */
case class ResourceUse(total: Int, available: Int) {
  require(total >= available)
}

object ResourceUse {
  implicit val resourceUseWrites: Writes[ResourceUse] = Json.writes[ResourceUse]
}
