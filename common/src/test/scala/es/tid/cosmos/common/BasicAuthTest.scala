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
