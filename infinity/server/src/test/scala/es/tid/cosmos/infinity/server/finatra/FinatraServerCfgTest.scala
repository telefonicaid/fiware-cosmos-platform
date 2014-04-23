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
