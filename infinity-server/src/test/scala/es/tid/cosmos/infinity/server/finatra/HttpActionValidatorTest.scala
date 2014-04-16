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

package es.tid.cosmos.infinity.server.finatra

import scalaz.{Failure, Success}

import com.twitter.finagle.http.Request
import org.jboss.netty.handler.codec.http.HttpMethod
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.server.actions.GetMetadata
import es.tid.cosmos.infinity.server.finatra.HttpActionValidator.InvalidResourcePath
import es.tid.cosmos.infinity.server.util.Path

class HttpActionValidatorTest extends FlatSpec with MustMatchers {

  "Valid HTTP Action" must "fail to extract from an unknown path" in {
    val req = Request(HttpMethod.GET, "/this/is/an/invalid/path")
    HttpActionValidator(req) must be (Failure(InvalidResourcePath("/this/is/an/invalid/path")))
  }

  it must "extract a GetPathMetadataAction" in {
    val req = Request(HttpMethod.GET, "/infinityfs/v1/metadata/path/to/file")
    HttpActionValidator(req) must be (Success(GetMetadata(Path.absolute("/path/to/file"))))
  }
}
