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

import org.scalatest.Assertions
import unfiltered.filter.{Plan, Planify}
import unfiltered.jetty.Http
import unfiltered.request.{Path => UPath, _}
import unfiltered.response._

import es.tid.cosmos.infinity.common.Path
import es.tid.cosmos.infinity.common.messages.{ErrorDescriptor, PathMetadata}
import es.tid.cosmos.infinity.common.messages.json.{ErrorDescriptorFormatter, MetadataFormatter}

class MockInfinityServer(metadataPort: Int) extends Assertions {

  val metadataEndpoint = new URL(s"http://localhost:$metadataPort")

  private var pathsMetadata = Map.empty[Path, PathMetadata]

  private val metadataFormatter = new MetadataFormatter()
  private val errorFormatter = new ErrorDescriptorFormatter()
  private val normalBehavior = Planify {
    case GET(UPath(MetadataPath(path))) =>
      pathsMetadata.get(Path.absolute(path)).fold(NotFound ~> errorBody("not found")) { metadata =>
        Ok ~> ResponseString(metadataFormatter.format(metadata))
      }

    case unexpected =>
      println(s"Unexpected request to ${unexpected.uri}")
      fail(s"Unexpected request to ${unexpected.uri}")
  }
  private var behavior: Plan = normalBehavior

  def givenExistingPath(metadata: PathMetadata): Unit = {
    pathsMetadata += metadata.path -> metadata
  }

  def givenWillReturnBadRequest(): Unit = {
    behavior = Planify {
      case _ => BadRequest ~> errorBody("don't understand you")
    }
  }

  def givenWillReturnGibberish(): Unit = {
    behavior = Planify {
      case _ => Ok ~> ResponseString("dlalkdkldijodjd")
    }
  }

  def givenWillReturnForbidden(): Unit = {
    behavior = Planify {
      case _ => Forbidden ~> errorBody("don't dare trying that")
    }
  }

  def withServer[T](block: => T): T = {
    val server = Http.local(metadataPort).filter(behavior)
    try {
      server.start()
      block
    } finally {
      server.stop().join()
    }
  }

  private val MetadataPath = """/infinityfs/v1/metadata(.*)""".r

  private def errorBody(description: String, code: String = "0") =
    ResponseString(errorFormatter.format(ErrorDescriptor(code, description)))
}
