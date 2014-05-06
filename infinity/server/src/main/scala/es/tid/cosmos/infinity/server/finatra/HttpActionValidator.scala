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
import org.apache.hadoop.hdfs.server.protocol.NamenodeProtocols
import org.jboss.netty.handler.codec.http.HttpMethod

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.common.json.{ParseException, RequestMessageParser}
import es.tid.cosmos.infinity.common.messages.{Request => ActionMessage}
import es.tid.cosmos.infinity.server.actions._
import es.tid.cosmos.infinity.server.config.InfinityConfig

/** An extractor object aimed to convert a Finagle HTTP request into a Infinity Server action. */
class HttpActionValidator(config: InfinityConfig, nameNode: NamenodeProtocols) {

  import scalaz.Scalaz._

  private val jsonParser = new RequestMessageParser()
  private val metadataUriPrefix = s"""${config.metadataBasePath}(/[^\\?]*)(\\?.*)?""".r

  def apply(request: Request): Validation[RequestParsingException, Action] =
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
        Delete(nameNode, absolutePath, request.getBooleanParam("recursive")).success
    }
  }

  private def postMetadataAction(path: String, request: Request) = {
    val content = request.getContentString()
    val absolutePath = Path.absolute(path)
    try {
      jsonParser.parse(content) match {
        case ActionMessage.CreateFile(name, perms, rep, bsize) =>
          CreateFile(config, nameNode, absolutePath / name, perms, rep, bsize).success
        case ActionMessage.ChangeOwner(owner) =>
          ChangeOwner(nameNode, absolutePath, owner).success
        case ActionMessage.ChangeGroup(group) =>
          ChangeGroup(nameNode, absolutePath, group).success
        case ActionMessage.ChangePermissions(permissions) =>
          ChangePermissions(nameNode, absolutePath, permissions).success
        case ActionMessage.Move(name, from) => {
          val on = Path.absolute(s"$path/$name")
          MoveFile(config, nameNode, on, from, MetadataUtil(nameNode)).success
        }
      }
    } catch {
      case e: ParseException =>
        RequestParsingException.InvalidRequestBody(content, e).failure
    }
  }
}
