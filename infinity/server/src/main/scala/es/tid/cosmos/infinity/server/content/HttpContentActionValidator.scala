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
import scalaz.Validation

import unfiltered.request._

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.actions._
import es.tid.cosmos.infinity.server.config.ContentServerConfig
import es.tid.cosmos.infinity.server.hadoop.DfsClientFactory
import es.tid.cosmos.infinity.server.errors.RequestParsingException

/** The validator for content actions.
  *
  * @param config           the content server configuration
  * @param dfsClientFactory the DFS client factory that will be used to access the underlying
  *                         file system
  */
class HttpContentActionValidator(
    config: ContentServerConfig,
    dfsClientFactory: DfsClientFactory) {
  //TODO: Consider moving actions, renderers and validators to metadata and content packages
  import scalaz.Scalaz._

  private val contentUriPrefix = s"""${config.localContentServerUrl.getPath}(/[^\\?]*)(\\?.*)?""".r

  def apply[T](request: HttpRequest[T]): Validation[RequestParsingException, Action] =
    request.uri match {
      case contentUriPrefix(path, _) => contentAction(path, request)
      case uri => RequestParsingException.InvalidResourcePath(uri).failure
    }

  private def contentAction[T](path: String, request: HttpRequest[T]) = {
    val absolutePath = Path.absolute(path)
    request match {
      case GET(Params(params)) => extractGetContentParams(params) match {
        case Success((offset, length)) =>
          GetContent(dfsClientFactory, absolutePath, offset, length).success
        case Failure(e) =>
          RequestParsingException.InvalidRequestParams(Seq("offset", "length"), e).failure
      }
      case POST(_) =>
        AppendContent(dfsClientFactory, absolutePath, request.inputStream, config.bufferSize).success
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
