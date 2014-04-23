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

import com.twitter.finatra.ResponseBuilder

sealed abstract class RequestError(errorCode: String, msg: String) extends Exception(msg) {

  def render: ResponseBuilder

  def toJson: String = s"""{ "errorCode" : "$errorCode", "cause" : "$msg" }"""
}

case class InvalidHttpCredentials(
    errorCode: String, msg: String) extends RequestError(errorCode, msg) {

  override def render = new ResponseBuilder()
    .status(401)
    .body(toJson)
    .header("WWW-Authenticate", "Basic realm=Cosmos")
    .header("WWW-Authenticate", "Bearer realm=Cosmos")
}

case class InvalidAction(errorCode: String, msg: String) extends RequestError(errorCode, msg) {

  override def render = new ResponseBuilder()
    .status(401)
    .body(toJson)
}
