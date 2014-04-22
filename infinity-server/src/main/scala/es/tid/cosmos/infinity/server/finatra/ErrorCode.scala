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

sealed trait ErrorCode {

  val code: String
}

case object MissingAuthorizationHeader extends ErrorCode {

  override val code = "101"

  def apply() = InvalidHttpCredentials(code, "no Authorization header was found in request")
}

case object UnsupportedAuthorizationHeader extends ErrorCode {

  override val code = "102"

  def apply(headerValue: String) = InvalidHttpCredentials(
    code, s"unsupported authorization header in '$headerValue'")
}

case object MalformedKeySecretPair extends ErrorCode {

  override val code = "103"

  def apply(pair: String) = InvalidHttpCredentials(code, s"invalid API key-secret pair in $pair")
}

case object InvalidBasicHash extends ErrorCode {

  override val code = "104"

  def apply(hash: String) = InvalidHttpCredentials(
    code, s"basic hash $hash is not a valid base64 encoded string")
}

case object InvalidResourcePath extends ErrorCode {

  override val code = "201"

  def apply(path: String) = InvalidAction(code, s"invalid resource path $path")
}
