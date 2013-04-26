package es.tid.cosmos.api.controllers

import play.api.libs.json.{Json, JsValue, Writes}

/**
 * @author sortega
 */
package object cosmos {

  case class ApiDescription(version: String)

  implicit object ApiDescriptionWrites extends Writes[ApiDescription] {
    def writes(desc: ApiDescription): JsValue = Json.obj(
      "version" -> desc.version
    )
  }
}
