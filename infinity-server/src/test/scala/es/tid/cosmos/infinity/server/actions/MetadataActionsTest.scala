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
package es.tid.cosmos.infinity.server.actions

import com.twitter.finatra.FinatraServer
import com.twitter.finatra.test.SpecHelper
import org.scalatest.FlatSpec
import org.scalatest.matchers.ShouldMatchers

class MetadataActionsTest extends FlatSpec with ShouldMatchers with SpecHelper {

    val app = new MetadataActions
    override val server = new FinatraServer
    server.register(app)

    "Get file metadata" should "respond 200" in {
      get("/infinityfs/v1/metadata/some/file.txt")
      response.body should equal ("metadata of /some/file.txt")
      response.code should equal (200)
    }

  }
