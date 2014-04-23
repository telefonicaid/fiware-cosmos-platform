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

import java.net.URL

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.common.Path
import es.tid.cosmos.infinity.common.messages._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class MetadataParserTest extends FlatSpec with MustMatchers {

  val parser = new MetadataParser()

  "A metadata parser" must "parse valid file metadata" in {
    parser.parse(
      """
        |{
        |  "path" : "/usr/gandalf/spells.txt",
        |  "type" : "file",
        |  "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/spells.txt",
        |  "content" : "http://example.com/infinityfs/v1/content/usr/gandalf/spells.txt",
        |  "owner" : "gandalf",
        |  "group" : "istari",
        |  "permissions" : "600",
        |  "size" : 45566918656,
        |  "modificationTime" : "2014-04-08T12:31:45+0100",
        |  "accessTime" : "2014-04-08T12:45:22+0100",
        |  "blockSize" : 65536,
        |  "replication" : 3
        |}
      """.stripMargin) must be (FileMetadata(
      path = Path.absolute("/usr/gandalf/spells.txt"),
      metadata = new URL("http://example.com/infinityfs/v1/metadata/usr/gandalf/spells.txt"),
      content = new URL("http://example.com/infinityfs/v1/content/usr/gandalf/spells.txt"),
      owner = "gandalf",
      group = "istari",
      permissions = PermissionsMask.fromOctal("600"),
      size = 45566918656L,
      modificationTime = Rfc822DateFormat.parse("2014-04-08T12:31:45+0100"),
      accessTime = Rfc822DateFormat.parse("2014-04-08T12:45:22+0100"),
      blockSize = 65536,
      replication = 3
    ))
  }

  it must "parse valid directory metadata" in {
    parser.parse(
      """
        |{
        |  "path" : "/usr/gandalf",
        |  "type" : "directory",
        |  "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf",
        |  "owner" : "gandalf",
        |  "group" : "istari",
        |  "permissions" : "755",
        |  "size" : 0,
        |  "modificationTime" : "2014-04-08T12:31:45+0100",
        |  "accessTime" : "2014-04-08T12:45:22+0100",
        |  "content" : [
        |    {
        |      "path" : "/usr/gandalf/spells.txt",
        |      "type" : "file",
        |      "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/spells.txt",
        |      "owner" : "gandalf",
        |      "group" : "istari",
        |      "permissions" : "600",
        |      "size" : 45566918656,
        |      "modificationTime" : "2014-04-08T12:41:34+0100",
        |      "accessTime" : "2014-04-08T12:54:32+0100"
        |    },
        |    {
        |      "path" : "/usr/gandalf/enemies",
        |      "type" : "directory",
        |      "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies",
        |      "owner" : "gandalf",
        |      "group" : "istari",
        |      "permissions" : "750",
        |      "size" : 0,
        |      "modificationTime" : "2014-04-08T12:55:45+0100",
        |      "accessTime" : "2014-04-08T13:01:22+0100"
        |    }
        |  ]
        |}
      """.stripMargin) must be (DirectoryMetadata(
      path = Path.absolute("/usr/gandalf"),
      metadata = new URL("http://example.com/infinityfs/v1/metadata/usr/gandalf"),
      owner = "gandalf",
      group = "istari",
      permissions = PermissionsMask.fromOctal("755"),
      modificationTime = Rfc822DateFormat.parse("2014-04-08T12:31:45+0100"),
      accessTime = Rfc822DateFormat.parse("2014-04-08T12:45:22+0100"),
      content = Seq(
        DirectoryEntry(
          path = Path.absolute("/usr/gandalf/spells.txt"),
          `type` = File,
          metadata = new URL("http://example.com/infinityfs/v1/metadata/usr/gandalf/spells.txt"),
          owner = "gandalf",
          group = "istari",
          modificationTime = Rfc822DateFormat.parse("2014-04-08T12:41:34+0100"),
          accessTime = Rfc822DateFormat.parse("2014-04-08T12:54:32+0100"),
          permissions = PermissionsMask.fromOctal("600"),
          size = 45566918656L
        ),
        DirectoryEntry(
          path = Path.absolute("/usr/gandalf/enemies"),
          `type` = Directory,
          metadata = new URL("http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies"),
          owner = "gandalf",
          group = "istari",
          modificationTime = Rfc822DateFormat.parse("2014-04-08T12:55:45+0100"),
          accessTime = Rfc822DateFormat.parse("2014-04-08T13:01:22+0100"),
          permissions = PermissionsMask.fromOctal("750"),
          size = 0L
        )
      )
    ))
  }

  it must "throw on malformed JSON" in {
    val ex = evaluating {
      parser.parse("{ not json]")
    } must produce [ParseException]
    ex.toString must include ("Malformed JSON")
  }

  it must "throw when fields are missing" in {
    val ex = evaluating {
      parser.parse(
        """
          |{
          |  "type" : "file",
          |  "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/spells.txt",
          |  "content" : "http://example.com/infinityfs/v1/content/usr/gandalf/spells.txt",
          |  "owner" : "gandalf",
          |  "group" : "istari",
          |  "permissions" : "600",
          |  "size" : 45566918656,
          |  "modificationTime" : "2014-04-08T12:31:45+0100",
          |  "accessTime" : "2014-04-08T12:45:22+0100",
          |  "blockSize" : 65536,
          |  "replication" : 3
          |}
        """.stripMargin)
    } must produce [ParseException]
    ex.toString must include ("Cannot map JSON to FileMetadata")
  }

  it must "throw when fields have invalid values" in {
    evaluating {
      parser.parse(
        """
          |{
          |  "path" : "/usr/gandalf/spells.txt",
          |  "type" : "file",
          |  "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/spells.txt",
          |  "content" : "http://example.com/infinityfs/v1/content/usr/gandalf/spells.txt",
          |  "owner" : "gandalf",
          |  "group" : "istari",
          |  "permissions" : "invalid permissions",
          |  "size" : 45566918656,
          |  "modificationTime" : "2014-04-08T12:31:45+0100",
          |  "accessTime" : "2014-04-08T12:45:22+0100",
          |  "blockSize" : 65536,
          |  "replication" : 3
          |}
        """.stripMargin)
    } must produce [ParseException]
  }

  it must "throw when type field is missing" in {
    val ex = evaluating {
      parser.parse(
        """
          |{
          |  "path" : "/usr/gandalf/spells.txt",
          |}
        """.stripMargin)
    } must produce [ParseException]
    ex.toString must include ("Missing 'type' field")
  }

  it must "throw when type field is other than file or directory" in {
    val ex = evaluating {
      parser.parse(
        """
          |{
          |  "path" : "/usr/gandalf/spells.txt",
          |  "type" : "pipe"
          |}
        """.stripMargin)
    } must produce [ParseException]
    ex.toString must include ("Unsupported metadata type 'pipe'")
  }
}
