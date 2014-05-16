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

package es.tid.cosmos.infinity.server.authorization

import java.net.InetAddress
import scala.util.Try

import unfiltered.request.{Authorization, RequestHeader, HttpRequest}

import es.tid.cosmos.infinity.server.errors.RequestParsingException

/** A request's authorization information.
  *
  * @param from   the request's origin host
  * @param header the request's authorization request
  */
case class AuthInfo(from: InetAddress, header: String) {
  require(!header.isEmpty, "Cannot have an empty authorization header")
}

object AuthInfo {
  def apply[T](
      from: String, request: HttpRequest[T]): Try[AuthInfo] = Try {
    request match {
      case Authorization(header) => AuthInfo(InetAddress.getByName(from), header)
      case _ => throw RequestParsingException.MissingAuthorizationHeader()
    }
  }
}
