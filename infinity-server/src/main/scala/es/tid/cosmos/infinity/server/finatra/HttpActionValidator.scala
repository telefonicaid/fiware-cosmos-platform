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

package es.tid.cosmos.infinity.server.finatra

import scalaz.Validation

import com.twitter.finagle.http.Request

import es.tid.cosmos.infinity.server.actions.{Action, GetMetadata}
import es.tid.cosmos.infinity.server.util.Path

/** An extractor object aimed to convert a Finagle HTTP request into a Infinity Server action. */
object HttpActionValidator {

  import scalaz.Scalaz._

  abstract class InvalidAction(msg: String) extends Exception(msg)

  case class InvalidResourcePath(path: String) extends InvalidAction(
    s"invalid resource path $path")

  val MetadataUriPrefix = "/infinityfs/v1/metadata(.*)".r

  def apply(request: Request): Validation[InvalidAction, Action] = request.getUri() match {
    case MetadataUriPrefix(path) => metadataAction(path, request)
    case uri => InvalidResourcePath(uri).failure
  }

  private def metadataAction(path: String, request: Request): Validation[InvalidAction, Action] =
    GetMetadata(Path.absolute(path)).success
}
