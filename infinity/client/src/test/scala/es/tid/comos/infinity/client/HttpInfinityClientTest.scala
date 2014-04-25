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

package es.tid.comos.infinity.client

import java.net.URL
import java.util.Date

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.Path
import es.tid.cosmos.infinity.common.messages.FileMetadata
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class HttpInfinityClientTest extends FlatSpec with MustMatchers with FutureMatchers {

  "An http Infinity client" must "retrieve metadata of an existing file" in new Fixture {
    val fileMetadata = FileMetadata(
      path = somePath,
      metadata = new URL("http://metadata/some/path"),
      content = new URL("http://content/some/path"),
      owner = "hdfs",
      group = "hdfs",
      modificationTime = new Date(1398420798000L),
      accessTime = new Date(1398420798000L),
      permissions = PermissionsMask.fromOctal("644"),
      replication = 3,
      blockSize = 2048,
      size = 250
    )
    infinity.givenExistingPath(fileMetadata)
    infinity.withServer {
      client.pathMetadata(somePath) must eventually(be(Some(fileMetadata)))
    }
  }

  it must "retrieve no metadata when file doesn't exist" in new Fixture {
    infinity.withServer {
      client.pathMetadata(somePath) must eventually(be('empty))
    }
  }

  it must "fail if connection cannot be established" in new Fixture {
    client.pathMetadata(somePath) must eventuallyFailWith[ConnectionException]
  }

  it must "fail with protocol mismatch exception on 400 responses" in new Fixture {
    infinity.givenWillReturnBadRequest()
    infinity.withServer {
      client.pathMetadata(somePath) must eventuallyFailWith[ProtocolMismatchException]
    }
  }

  it must "fail with protocol mismatch exception for unparseable responses" in new Fixture {
    infinity.givenWillReturnGibberish()
    infinity.withServer {
      client.pathMetadata(somePath) must eventuallyFailWith[ProtocolMismatchException]
    }
  }

  it must "fail with forbidden exception for 403 responses" in new Fixture {
    infinity.givenWillReturnForbidden()
    infinity.withServer {
      client.pathMetadata(somePath) must eventuallyFailWith[ForbiddenException]
    }
  }

  trait Fixture {
    val somePath = Path.absolute("/some/path")
    val infinity = new MockInfinityServer(metadataPort = 8898)
    val client = new HttpInfinityClient(infinity.metadataEndpoint)
  }
}
