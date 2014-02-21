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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import org.mockito.BDDMockito.given
import org.mockito.Matchers.any
import play.api.libs.json.{JsError, JsSuccess, Reads, JsValue}

class JsonProfileParserTest extends FlatSpec with MustMatchers with MockitoSugar {

  val profileReads = mock[Reads[OAuthUserProfile]]
  val parser = new JsonProfileParser(profileReads)

  "A JSON profile parser" must "throw when parsing invalid JSON documents" in {
    evaluating {
      parser.parse("{ invalid json }")
    } must produce [InvalidProfileException]
  }

  it must "parse valid JSON documents" in {
    val profile = mock[OAuthUserProfile]
    given(profileReads.reads(any[JsValue])).willReturn(JsSuccess(profile))
    parser.parse("{}") must be (profile)
  }

  it must "throw when JSON document doesn't represent a user profile" in {
    given(profileReads.reads(any[JsValue])).willReturn(JsError("invalid"))
    evaluating {
      parser.parse("{}")
    } must produce [InvalidProfileException]
  }
}
