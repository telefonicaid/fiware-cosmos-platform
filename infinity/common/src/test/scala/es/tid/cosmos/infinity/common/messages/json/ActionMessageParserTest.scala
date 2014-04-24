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

package es.tid.cosmos.infinity.common.messages.json

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.common.Path
import es.tid.cosmos.infinity.common.messages._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class ActionMessageParserTest extends FlatSpec with MustMatchers {

  val parser = new ActionMessageParser()

  "An action message parser" must "parse a create file action" in {
    parser.parse(
      """
        |{
        |  "action" : "mkfile",
        |  "name" : "enemies.csv",
        |  "permissions" : "640",
        |  "replication" : 2,
        |  "blockSize" : 67108864
        |}
      """.stripMargin
    ) must be (CreateFile(
      name = "enemies.csv",
      permissions = PermissionsMask.fromOctal("640"),
      replication = Some(2),
      blockSize = Some(67108864)
    ))
  }

  it must "parse a create file action lacking optional fields" in {
    parser.parse(
      """
        |{
        |  "action" : "mkfile",
        |  "name" : "enemies.csv",
        |  "permissions" : "640"
        |}
      """.stripMargin
    ) must be (CreateFile(
      name = "enemies.csv",
      permissions = PermissionsMask.fromOctal("640"),
      replication = None,
      blockSize = None
    ))
  }

  it must "parse a create directory action" in {
    parser.parse(
      """
        |{
        |  "action" : "mkdir",
        |  "name" : "enemies",
        |  "permissions" : "750"
        |}
      """.stripMargin
    ) must be (CreateDirectory(
      name = "enemies",
      permissions = PermissionsMask.fromOctal("750")
    ))
  }

  it must "parse a move action" in {
    parser.parse(
      """
        |{
        |  "action" : "move",
        |  "name" : "enemies",
        |  "from" : "/usr/sauron/friends",
        |}
      """.stripMargin
    ) must be (Move(
      name = "enemies",
      from = Path.absolute("/usr/sauron/friends")
    ))
  }

  it must "parse a change owner action" in {
    parser.parse(
      """
        |{
        |  "action" : "chown",
        |  "owner" : "theoden"
        |}
      """.stripMargin
    ) must be (ChangeOwner("theoden"))
  }

  it must "parse a change group action" in {
    parser.parse(
      """
        |{
        |  "action" : "chgrp",
        |  "group" : "valar"
        |}
      """.stripMargin
    ) must be (ChangeGroup("valar"))
  }

  it must "parse a change permissions action" in {
    parser.parse(
      """
        |{
        |  "action" : "chmod",
        |  "permissions" : "755"
        |}
      """.stripMargin
    ) must be (ChangePermissions(PermissionsMask.fromOctal("755")))
  }

  it must "not parse an action without field 'action'" in {
    val ex = evaluating(parser.parse("{}")) must produce [ParseException]
    ex.toString must include ("Missing 'action' field")
  }

  it must "not parse an action with unknown name" in {
    val ex = evaluating(parser.parse("""{ "action": "foo" }""")) must produce [ParseException]
    ex.toString must include ("Unsupported action 'foo'")
  }

  it must "not parse an action with values out of range" in {
    val ex = evaluating {
      parser.parse(
        """
          |{
          |  "action" : "mkfile",
          |  "name" : "enemies.csv",
          |  "permissions" : "640",
          |  "replication" : -1
          |}
        """.stripMargin)
    } must produce [ParseException]
    ex.toString must include ("Cannot map JSON to CreateFile")
  }
}
