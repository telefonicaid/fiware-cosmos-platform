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

package es.tid.cosmos.common

import org.apache.commons.codec.binary.Base64
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class BasicAuthTest extends FlatSpec with MustMatchers {

  "BasicAuth" must "support roundtrip conversion" in {
    BasicAuth.unapply(BasicAuth("user", "pass")) must be (Some(("user", "pass")))
  }

  "An authorization header" must "not be extracted for non-basic headers" in {
    BasicAuth.unapply("OtherScheme ABCDE1234") must not be 'defined
  }

  it must "not be extracted for non-base64 values" in {
    BasicAuth.unapply("Basic ññññññ") must not be 'defined
  }

  it must "not be extracted for values with other than two fields" in {
    val value = Base64.encodeBase64String("field1:field2:field3".getBytes)
    BasicAuth.unapply(s"Basic $value") must not be 'defined
  }

  it must "be extracted from valid headers" in {
    val value = Base64.encodeBase64String("user:pass".getBytes)
    BasicAuth.unapply(s"Basic $value") must be (Some("user", "pass"))
  }
}
