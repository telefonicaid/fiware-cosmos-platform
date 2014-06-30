/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.server.content

import scala.util.Try
import scalaz.ValidationNel

import unfiltered.request._

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.actions._
import es.tid.cosmos.infinity.server.config.ContentServerConfig
import es.tid.cosmos.infinity.server.errors.RequestParsingException
import es.tid.cosmos.infinity.server.hadoop.DataNode

/** The validator for content actions.
  *
  * @param config   the content server configuration
  * @param dataNode the DataNode API
  */
class HttpContentActionValidator(
    config: ContentServerConfig,
    dataNode: DataNode) {
  //TODO: Consider moving actions, renderers and validators to metadata and content packages
  import HttpContentActionValidator._

  private val contentUriPrefix = s"""${config.localContentServerUrl.getPath}(/[^\\?]*)(\\?.*)?""".r

  def apply[T](request: HttpRequest[T]): Try[ContentAction] = Try {
    request.uri match {
      case contentUriPrefix(path, _) => contentAction(path, request)
      case uri => throw RequestParsingException.InvalidResourcePath(uri)
    }
  }

  private def contentAction[T](path: String, request: HttpRequest[T]) = {
    val absolutePath = Path.absolute(path)
    request match {
      case GET(Params(params)) => extractGetContentParams(params).fold(
        succ = { case (offset, length) =>
          GetContent(dataNode, absolutePath, offset, length) },
        fail = failedParams =>
          throw RequestParsingException.InvalidRequestParams(failedParams.list)
      )
      case POST(_) =>
        AppendContent(dataNode, absolutePath, request.inputStream)
      case PUT(_) =>
        OverwriteContent(dataNode, absolutePath, request.inputStream)
    }
  }

  private def extractGetContentParams(
      params: Map[String, Seq[String]]): ValidationNel[String, OffsetLengthParams] = {
    import scalaz.Scalaz._

    def toValidLong(key: String, condition: Long => Boolean) = {
      val maybeValue = params.get(key) map (_.head.toLong)
      if (maybeValue.isEmpty || maybeValue.exists(condition))
        maybeValue.successNel
      else
        key.failureNel
    }

    val offset = toValidLong("offset", isNatural)
    val length = toValidLong("length", isPositive)
    (offset |@| length){(_, _)}
  }
}

private object HttpContentActionValidator {
  type OffsetLengthParams = (Option[Long], Option[Long])
  def isNatural(n: Long) = n >= 0
  def isPositive(n: Long) = n > 0
}
