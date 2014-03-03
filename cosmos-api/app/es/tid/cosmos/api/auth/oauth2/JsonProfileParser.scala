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

package es.tid.cosmos.api.auth.oauth2

import com.fasterxml.jackson.core.JsonParseException
import play.api.libs.json._
import play.api.libs.json.JsSuccess

/** Parse of JSON-based profiles. */
class JsonProfileParser(profileReads: Reads[OAuthUserProfile]) extends ProfileParser {

  override def parse(input: String): OAuthUserProfile = {
    parseProfile(parseJson(input), input)
  }

  private def parseJson(input: String): JsValue = try {
    Json.parse(input)
  } catch {
    case jsonEx: JsonParseException =>
      throw InvalidProfileException(input, Seq(jsonEx.getMessage), jsonEx)
  }

  private def parseProfile(json: JsValue, originalInput: String): OAuthUserProfile = {
    Json.fromJson(json)(profileReads) match {
      case JsSuccess(profile, _) => profile
      case JsError(errors) =>
        throw InvalidProfileException(originalInput, errors.map(_.toString()))
    }
  }
}
