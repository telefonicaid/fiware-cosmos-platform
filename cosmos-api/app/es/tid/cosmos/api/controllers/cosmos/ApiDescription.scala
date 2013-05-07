package es.tid.cosmos.api.controllers.cosmos

import play.api.libs.json.{Json, JsValue, Writes}

/**
 * Represents general information about the API.
 *
 * @param version API version number
 */
case class ApiDescription(version: String)

object ApiDescription {
  implicit object ApiDescriptionWrites extends Writes[ApiDescription] {
    def writes(desc: ApiDescription): JsValue = Json.obj(
      "version" -> desc.version
    )
  }
}