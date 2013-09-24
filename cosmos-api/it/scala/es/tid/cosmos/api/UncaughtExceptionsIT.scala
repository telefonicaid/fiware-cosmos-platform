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

package es.tid.cosmos.api

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.http.MimeTypes.{JSON, HTML}
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.mocks.WithTestApplication

class UncaughtExceptionsIT extends FlatSpec with MustMatchers {

  val ex = new IllegalArgumentException("Sample exception")
  val pageRequest = FakeRequest(GET, "/")
  val apiRequest = FakeRequest(POST, "/cosmos/v1/cluster")

  "A uncaught exception" must "be rendered as HTML by default for page routes" in
    new WithTestApplication {
      contentType(playGlobal.onError(pageRequest, ex)) must be (Some(HTML))
    }

  it must "be rendered as JSON by default for API routes" in new WithTestApplication {
    contentType(playGlobal.onError(apiRequest, ex)) must be (Some(JSON))
  }

  it must "be rendered as HTML when asked for it" in new WithTestApplication {
    val request = apiRequest.withHeaders("Accept" -> HTML)
    contentType(playGlobal.onError(request, ex)) must be (Some(HTML))
  }

  it must "be rendered as JSON when asked for it" in new WithTestApplication {
    val request = pageRequest.withHeaders("Accept" -> JSON)
    contentType(playGlobal.onError(request, ex)) must be (Some(JSON))
  }
}
