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

package es.tid.cosmos.api.controllers

import play.api.libs.json.{Json, JsValue, Writes}

package object cosmos {
  case class ApiDescription(version: String)

  implicit object ApiDescriptionWrites extends Writes[ApiDescription] {
    def writes(desc: ApiDescription): JsValue = Json.obj(
      "version" -> desc.version
    )
  }
}
