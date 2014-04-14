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

import org.scalatest.FlatSpec
import org.scalatest.matchers.ShouldMatchers

class FinatraServerCfgTest extends FlatSpec with ShouldMatchers {

  "FinatraServerCfg" must "reject config without endpoint" in {
    intercept[IllegalArgumentException] {
      FinatraServerCfg()
    }
  }

  it must "reject incomplet https configuration" in {
    intercept[IllegalArgumentException] {
      FinatraServerCfg(https = Some("0.0.0.0:443"), certificatePath = Some("/path/file"))
    }
  }

  it must "accept valid http config" in {
    FinatraServerCfg(http = Some("0.0.0.0:80"))
  }

  it must "accept valid https config" in {
    FinatraServerCfg(https = Some("0.0.0.0:443"),
      certificatePath = Some("/path/file"),
      keyPath = Some("/path/other-file"))
  }

}
