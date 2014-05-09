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

package es.tid.cosmos.infinity.server.finatra

import scala.util.{Try, Success, Failure}
import scalaz.Validation

import com.twitter.finagle.http.Request
import org.jboss.netty.handler.codec.http.HttpMethod

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.actions.{GetContent, Action}
import es.tid.cosmos.infinity.server.config.InfinityContentServerConfig
import es.tid.cosmos.infinity.server.hadoop.DfsClientFactory

/** The validator for content actions.
  *
  * @param config           the content server configuration
  * @param dfsClientFactory the DFS client factory that will be used to access the underlying
  *                         file system
  */
class HttpContentActionValidator(
    config: InfinityContentServerConfig,
    dfsClientFactory: DfsClientFactory) {
  //TODO: Consider moving actions, renderers and validators to metadata and content packages
  import scalaz.Scalaz._

  private val contentUriPrefix = s"""${config.contentServerUrl.getPath}(/[^\\?]*)(\\?.*)?""".r

  def apply(request: Request): Validation[RequestParsingException, Action] =
    request.getUri() match {
      case contentUriPrefix(path, _) => contentAction(path, request)
      case uri => RequestParsingException.InvalidResourcePath(uri).failure
    }

  private def contentAction(path: String, request: Request) = {
    val absolutePath = Path.absolute(path)
    request.method match {
      case HttpMethod.GET => extractGetContentParams(request) match {
        case Success((offset, length)) =>
          GetContent(dfsClientFactory, absolutePath, offset, length).success
        case Failure(e) =>
          RequestParsingException.InvalidRequestParams(Seq("offset", "length"), e).failure
      }
    }
  }

  private def extractGetContentParams(request: Request): Try[(Option[Long], Option[Long])] = Try {
    def toValidLong(s: String, condition: Long => Boolean, message: String): Long = {
      val n = s.toLong
      require(condition(n), s"$message. Found: $n")
      n
    }
    val optionalOffset =
      Option(request.getParam("offset")) map (toValidLong(_, _ >= 0, s"offset cannot be negative"))
    val optionalLength =
      Option(request.getParam("length")) map (toValidLong(_, _ > 0, s"length must be positive"))

    (optionalOffset, optionalLength)
  }
}
