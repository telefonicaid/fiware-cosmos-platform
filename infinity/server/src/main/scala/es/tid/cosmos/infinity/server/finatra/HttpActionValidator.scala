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

import es.tid.cosmos.infinity.common.Path
import es.tid.cosmos.infinity.server.actions.{Delete, Action, GetMetadata}

/** An extractor object aimed to convert a Finagle HTTP request into a Infinity Server action. */
class HttpActionValidator(nameNode: NamenodeProtocols) {

  import scalaz.Scalaz._

  val MetadataUriPrefix = "/infinityfs/v1/metadata(.*)".r

  def apply(request: Request): Validation[RequestParsingException, Action] =
    request.getUri() match {
      case MetadataUriPrefix(path) => metadataAction(path, request)
      case uri => RequestParsingException.InvalidResourcePath(uri).failure
    }

  private def metadataAction(path: String, request: Request) = {
    val absolutePath = Path.absolute(path)
    request.method match {
      case HttpMethod.GET => GetMetadata(nameNode, absolutePath).success
      case HttpMethod.DELETE =>
        Delete(nameNode, absolutePath, request.getBooleanParam("recursive")).success
    }
  }
}
