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
package es.tid.cosmos.infinity.server.actions

import com.twitter.finatra.{Request, Controller}

class MetadataActions extends Controller {

  import MetadataActions._

  val contentPath = Prefix + "/*"

  get(contentPath) { request =>
    val path = getPath(request)
    render.plain(s"metadata of $path").toFuture
  }

}

object MetadataActions {
  val Prefix = "/infinityfs/v1/metadata"

  // `splat` is an undocumented Finatra tag for wilcards routes extractions
  // see https://github.com/twitter/finatra/blob/master/src/main/scala/com/twitter/finatra/PathParser.scala
  def getPath(request: Request): String = "/" + request.routeParams.getOrElse("splat", "")
}
