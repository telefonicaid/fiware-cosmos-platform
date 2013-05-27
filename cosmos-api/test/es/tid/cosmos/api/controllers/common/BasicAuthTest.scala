package es.tid.cosmos.api.controllers.common

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import com.ning.http.util.Base64

class BasicAuthTest extends FlatSpec with MustMatchers {
  "BasicAuth" must "support roundtrip conversion" in {
    BasicAuth.unapply(BasicAuth("realm", "pass")) must be (Some(("realm", "pass")))
  }

  "An authorization header" must "not be extracted for non-basic headers" in {
    BasicAuth.unapply("OtherScheme ABCDE1234") must not be ('defined)
  }

  it must "not be extracted for non-base64 values" in {
    BasicAuth.unapply("Basic ññññññ") must not be ('defined)
  }

  it must "not be extracted for values with more than two fields" in {
    val value = Base64.encode("field1:field2:field3".getBytes)
    BasicAuth.unapply(s"Basic $value") must not be ('defined)
  }

  it must "be extracted from valid headers" in {
    val value = Base64.encode("user:pass".getBytes)
    BasicAuth.unapply(s"Basic $value") must be (Some("user", "pass"))
  }
}
