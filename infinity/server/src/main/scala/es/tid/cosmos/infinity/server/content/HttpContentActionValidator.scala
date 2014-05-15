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

import scala.util.{Try, Success, Failure}

import unfiltered.request._

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.actions._
import es.tid.cosmos.infinity.server.config.ContentServerConfig
import es.tid.cosmos.infinity.server.hadoop.{DataNode, DfsClientFactory}
import es.tid.cosmos.infinity.server.errors.RequestParsingException

/** The validator for content actions.
  *
  * @param config   the content server configuration
  * @param dataNode the DataNode API
  */
class HttpContentActionValidator(
    config: ContentServerConfig,
    dataNode: DataNode) {
  //TODO: Consider moving actions, renderers and validators to metadata and content packages

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
      case GET(Params(params)) => extractGetContentParams(params) match {
        case Success((offset, length)) =>
          GetContent(dataNode, absolutePath, offset, length)
        case Failure(e) =>
          throw RequestParsingException.InvalidRequestParams(Seq("offset", "length"), e)
      }
      case POST(_) =>
        AppendContent(dataNode, absolutePath, request.inputStream)
      case PUT(_) =>
        OverwriteContent(dataNode, absolutePath, request.inputStream)
    }
  }

  private def extractGetContentParams(params: Map[String, Seq[String]]): Try[(Option[Long], Option[Long])] = Try {
    def toValidLong(s: String, condition: Long => Boolean, message: String): Long = {
      val n = s.toLong
      require(condition(n), s"$message. Found: $n")
      n
    }
    // TODO: what happens when params passed are seqs? Test against it!
    val optionalOffset =
      params.get("offset") map (_.head) map (toValidLong(_, _ >= 0, s"offset cannot be negative"))
    val optionalLength =
      params.get("length") map (_.head) map (toValidLong(_, _ > 0, s"length must be positive"))

    (optionalOffset, optionalLength)
  }
}
