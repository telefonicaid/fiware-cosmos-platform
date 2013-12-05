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

package es.tid.cosmos.api.controllers.cluster

import play.api.libs.functional.syntax._
import play.api.libs.json._

/** Parameters for cluster user management actions (add and remove user).  */
case class ManageUserParams(user: String)

object ManageUserParams {

  implicit val manageUserParamsFormat = Json.format[ManageUserParams]
}
