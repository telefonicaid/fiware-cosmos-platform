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

import net.liftweb.json._
import net.liftweb.json.JsonDSL._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.common.Path
import es.tid.cosmos.infinity.common.messages.Action._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class ActionFormatterTest extends FlatSpec with MustMatchers {

  val formatter = new ActionMessageFormatter

  "An action message formatter" must "format a create file action" in {
    val action = CreateFile(
      name = "enemies.csv",
      permissions = PermissionsMask.fromOctal("640"),
      replication = Some(2),
      blockSize = Some(67108864)
    )
    parse(formatter.format(action)) must be (
      ("action" -> "mkfile") ~
        ("name" -> "enemies.csv") ~
        ("permissions" -> "640") ~
        ("replication" -> 2) ~
        ("blockSize" -> 67108864)
    )
  }

  it must "format a create file action lacking optional fields" in {
    val action = CreateFile(
      name = "enemies.csv",
      permissions = PermissionsMask.fromOctal("640"),
      replication = None,
      blockSize = None
    )
    parse(formatter.format(action)) must be (
      ("action" -> "mkfile") ~
        ("name" -> "enemies.csv") ~
        ("permissions" -> "640")
    )
  }

  it must "format a create directory action" in {
    val action = CreateDirectory(
      name = "enemies",
      permissions = PermissionsMask.fromOctal("750")
    )
    parse(formatter.format(action)) must be (
      ("action" -> "mkdir") ~
        ("name" -> "enemies") ~
        ("permissions" -> "750")
    )
  }

  it must "format a move action" in {
    val action = Move(
      name = "enemies",
      from = Path.absolute("/usr/sauron/friends")
    )
    parse(formatter.format(action)) must be (
      ("action" -> "move") ~
        ("name" -> "enemies") ~
        ("from" -> "/usr/sauron/friends")
    )
  }

  it must "format a change owner action" in {
    parse(formatter.format(ChangeOwner("theoden"))) must be (
      ("action" -> "chown") ~ ("owner" -> "theoden")
    )
  }

  it must "format a change group action" in {
    parse(formatter.format(ChangeGroup("valar"))) must be (
      ("action" -> "chgrp") ~ ("group" -> "valar")
    )
  }

  it must "format a change permissions action" in {
    parse(formatter.format(ChangePermissions(PermissionsMask.fromOctal("700")))) must be (
      ("action" -> "chmod") ~ ("permissions" -> "700")
    )
  }
}
