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

package es.tid.cosmos.api.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class ClusterSecretTest extends FlatSpec with MustMatchers {

  "A cluster secret" must "have the right length" in {
    evaluating {
      ClusterSecret("X" * (ClusterSecret.Length + 1))
    } must produce [IllegalArgumentException]
    evaluating {
      ClusterSecret("X" * (ClusterSecret.Length - 1))
    } must produce [IllegalArgumentException]
  }

  it must "contain only alphanumeric characters" in {
    for (invalidChar <- Seq("_", " ", "ñ")) {
      val invalidSecret = invalidChar * ClusterSecret.Length
      evaluating {
        ClusterSecret(invalidSecret)
      } must produce [IllegalArgumentException]
    }
  }

  it must "avoid leaking the secret when casually converted to string" in {
    val rawSecret = Seq.tabulate(ClusterSecret.Length)(index => index % 10).mkString("")
    val secret = ClusterSecret(rawSecret)
    secret.toString must not include rawSecret
  }

  it must "be generated randomly" in {
    ClusterSecret.random() must not be ClusterSecret.random()
  }
}
