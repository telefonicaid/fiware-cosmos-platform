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

package es.tid.cosmos.infinity.server.finatra

import scalaz.Validation

import com.twitter.finagle.http.Request
import org.jboss.netty.handler.codec.http.HttpMethod

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.common.json.{ParseException, RequestMessageParser}
import es.tid.cosmos.infinity.common.messages.{Request => RequestMessage}
import es.tid.cosmos.infinity.server.actions._
import es.tid.cosmos.infinity.server.config.InfinityConfig
import es.tid.cosmos.infinity.server.errors.RequestParsingException
import es.tid.cosmos.infinity.server.hadoop.NameNode

/** An extractor object aimed to convert a Finagle HTTP request into a Infinity Server action. */
class HttpActionValidator(config: InfinityConfig, nameNode: NameNode) {

  import scalaz.Scalaz._

  private val jsonParser = new RequestMessageParser()
  private val metadataUriPrefix = s"""${config.metadataBasePath}(/[^\\?]*)(\\?.*)?""".r

  def apply(request: Request): Validation[RequestParsingException, MetadataAction] =
    request.getUri() match {
      case metadataUriPrefix(path, _) => metadataAction(path, request)
      case uri => RequestParsingException.InvalidResourcePath(uri).failure
    }

  private def metadataAction(path: String, request: Request) = {
    val absolutePath = Path.absolute(path)
    request.method match {
      case HttpMethod.GET =>
        GetMetadata(nameNode, absolutePath).success
      case HttpMethod.POST =>
        postMetadataAction(path, request)
      case HttpMethod.DELETE =>
        DeletePath(nameNode, absolutePath, request.getBooleanParam("recursive")).success
    }
  }

  private def postMetadataAction(path: String, request: Request) = {
    val content = request.getContentString()
    val absolutePath = Path.absolute(path)
    try {
      jsonParser.parse(content) match {
        case RequestMessage.CreateFile(name, perms, rep, bsize) =>
          CreateFile(config, nameNode, absolutePath / name, perms, rep, bsize).success
        case RequestMessage.CreateDirectory(name, perms) =>
          CreateDirectory(nameNode, absolutePath / name, perms).success
        case RequestMessage.ChangeOwner(owner) =>
          ChangeOwner(nameNode, absolutePath, owner).success
        case RequestMessage.ChangeGroup(group) =>
          ChangeGroup(nameNode, absolutePath, group).success
        case RequestMessage.ChangePermissions(permissions) =>
          ChangePermissions(nameNode, absolutePath, permissions).success
        case RequestMessage.Move(name, from) =>
          MovePath(config, nameNode, Path.absolute(s"$path/$name"), from).success
      }
    } catch {
      case e: ParseException =>
        RequestParsingException.InvalidRequestBody(content, e).failure
    }
  }
}
