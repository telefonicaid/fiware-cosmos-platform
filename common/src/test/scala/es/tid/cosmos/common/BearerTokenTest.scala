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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class BearerTokenTest extends FlatSpec with MustMatchers {

  "Bearer token" must "be formatted as a header" in {
    BearerToken("token") must be ("Bearer token")
  }

  it must "be extracted from a valid header" in {
    BearerToken.unapply("Bearer 1a.2b_3c-4d+5e/6f") must be (Some("1a.2b_3c-4d+5e/6f"))
  }

  it must "consider the scheme in a case insensitive fashion" in {
    BearerToken.unapply("BEARER 1a.2b_3c-4d+5e/6f") must be (Some("1a.2b_3c-4d+5e/6f"))
  }

  it must "not be extracted if scheme is not 'bearer'" in {
    BearerToken.unapply("basic 1a.2b_3c-4d+5e/6f") must be ('empty)
  }

  it must "not be extracted if there is no token" in {
    BearerToken.unapply("bearer ") must be ('empty)
  }
}
