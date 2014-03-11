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
