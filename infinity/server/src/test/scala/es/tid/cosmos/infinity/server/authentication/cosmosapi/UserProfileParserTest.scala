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

package es.tid.cosmos.infinity.server.authentication.cosmosapi

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.common.permissions.{PermissionsMask, UserProfile}

class UserProfileParserTest extends FlatSpec with MustMatchers {

  val parser = new UserProfileParser(superGroup = "hdfs")

  "User profile" must "be parsed from JSON" in {
    parser.parse(
      """{
        | "user": "gandalf",
        | "group": "istari",
        | "accessMask": "752",
        | "origins": ["orion01", "orion02"]
        |}
      """.stripMargin
    ) must be (UserProfile(
      username = "gandalf",
      group = "istari",
      mask = PermissionsMask.fromOctal("752"),
      accessFrom = Some(Set("orion01", "orion02"))
    ))
  }

  it must "be parsed as root if the group is the superuser group" in {
    parser.parse(
      """{
        | "user": "gandalf",
        | "group": "hdfs",
        | "accessMask": "752",
        |}
      """.stripMargin
    ) must be ('superuser)
  }

  it must "be parsed when optional fields are missing" in {
    parser.parse(
      """{
        | "user": "gandalf",
        | "group": "istari",
        | "accessMask": "752"
        |}
      """.stripMargin
    ) must be (UserProfile(
      username = "gandalf",
      group = "istari",
      mask = PermissionsMask.fromOctal("752")
    ))
  }

  it must "fail to be parsed when required fields are missing" in {
    evaluating {
      parser.parse(
        """{
          | "user": "gandalf",
          |}
        """.stripMargin
      )
    } must produce [IllegalArgumentException]
  }

  it must "fail to be parsed with invalid access mask value" in {
    evaluating {
      parser.parse(
        """{
          | "user": "gandalf",
          | "group": "istari",
          | "accessMask": "This is a invalid access mask value"
          |}
        """.stripMargin
      )
    } must produce [IllegalArgumentException]
  }

  it must "fail to be parsed from invalid JSON" in {
    evaluating {
      parser.parse("This is a stupid message that does not mean anything!")
    } must produce [IllegalArgumentException]
  }
}
