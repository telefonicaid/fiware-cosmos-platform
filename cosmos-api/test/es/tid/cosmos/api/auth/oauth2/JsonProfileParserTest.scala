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
