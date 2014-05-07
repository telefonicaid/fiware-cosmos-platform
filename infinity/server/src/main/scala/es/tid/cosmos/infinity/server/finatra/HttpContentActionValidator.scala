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
import org.apache.hadoop.hdfs.server.datanode.DataNode
import org.jboss.netty.handler.codec.http.HttpMethod

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.common.json.RequestMessageParser
import es.tid.cosmos.infinity.server.actions.{GetContent, Action}
import es.tid.cosmos.infinity.server.config.InfinityContentServerConfig

/**
 * TODO: Insert description here
 *
 */
class HttpContentActionValidator(config: InfinityContentServerConfig, dataNode: DataNode) {
  //TODO: Consider moving actions to metadata and content packages
  // TODO: Extract common code between content and metadata plugin server and routes
  import scalaz.Scalaz._

//  private val jsonParser = new RequestMessageParser()
  private val contentUriPrefix = s"""${config.contentServerUrl.getPath}(/[^\\?]*)(\\?.*)?""".r

  def apply(request: Request): Validation[RequestParsingException, Action] =
    request.getUri() match {
      case contentUriPrefix(path, _) => contentAction(path, request)
      case uri => RequestParsingException.InvalidResourcePath(uri).failure
    }


  private def contentAction(path: String, request: Request) = {
    val absolutePath = Path.absolute(path)
    request.method match {
      case HttpMethod.GET => {
        extractGetContentParams(request) match {
          case Success((offset, length)) => GetContent(dataNode, absolutePath, offset, length).success
          case Failure(e) => RequestParsingException.InvalidRequestParams(Seq("offset", "length"), e).failure
        }
      }
//      case HttpMethod.POST =>
//        postMetadataAction(path, request)
//      case HttpMethod.DELETE =>
//        Delete(nameNode, absolutePath, request.getBooleanParam("recursive")).success
    }
  }

  private def extractGetContentParams(request: Request): Try[(Option[Long], Option[Long])] = Try {
    def toValidLong(s: String, condition: Long => Boolean, message: String): Long = {
      val n = s.toLong
      require(condition(n), s"$message. Found: $n")
      n
    }
    val optionalOffset = Option(request.getParam("offset")) map (toValidLong(_, _ >= 0, s"offset cannot be negative"))
    val optionalLength = Option(request.getParam("length")) map (toValidLong(_, _ > 0, s"length must be positive"))
    (optionalOffset, optionalLength)
  }
}
