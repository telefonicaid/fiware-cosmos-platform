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

package es.tid.cosmos.infinity.server.groups

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.server.groups.UserGroups.InvalidUserGroups

class UserGroupsParseTest extends FlatSpec with MustMatchers {

  "User groups parsing" must "parse valid JSONs" in {
    UserGroups.parse(
      """
        |{
        |  "groups": ["cosmos", "alpha"]
        |}
      """.stripMargin) must be (Seq("cosmos", "alpha"))
  }

  it must "fail for invalid JSONs" in {
    val ex = evaluating {
      UserGroups.parse("not a JSON")
    } must produce [InvalidUserGroups]
    ex.toString must include ("Not a JSON")
  }

  it must "fail when JSON doesn't have a groups field" in {
    val ex = evaluating {
      UserGroups.parse(
        """
          |{
          |  "different": "format"
          |}
        """.stripMargin)
    } must produce [InvalidUserGroups]
    ex.toString must include ("Invalid JSON")
  }

  it must "fail if groups are not simple strings" in {
    val ex = evaluating {
      UserGroups.parse(
        """
          |{
          |  "groups": ["a", 2, {}]
          |}
        """.stripMargin)
    } must produce [InvalidUserGroups]
    ex.toString must include ("Invalid JSON")
  }
}
