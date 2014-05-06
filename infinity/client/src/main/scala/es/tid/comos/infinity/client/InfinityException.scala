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

package es.tid.comos.infinity.client

import java.net.URL
import scala.concurrent.duration.Duration

import es.tid.cosmos.infinity.common.Path
import es.tid.cosmos.infinity.common.messages.ErrorDescriptor

/** Base class for exceptions related Infinity API use */
sealed abstract class InfinityException(
    description: String, code: Option[String] = None, cause: Throwable = null)
  extends Exception(code.mkString("Code ", "", ": ") + description, cause) {

  def this(description: String, cause: Throwable) = this(description, None, cause)

  def this(error: ErrorDescriptor) = this(error.code, Some(error.cause))
}

case class ConnectionException(endpoint: URL, cause: Throwable = null)
  extends InfinityException(s"Cannot connect to Infinity at $endpoint", cause)

/** Client and server don't understand each other. */
case class ProtocolMismatchException(
    description: String, code: Option[String] = None, cause: Throwable = null)
  extends InfinityException(
    s"Protocol mismatch: $description. Check that versions of client and server match", code, cause)

/** Credentials being used have been rejected */
case class UnauthorizedException(error: ErrorDescriptor) extends InfinityException(error)

/** User access rights are not enough to perform an action */
case class ForbiddenException(error: ErrorDescriptor) extends InfinityException(error)

case class AlreadyExistsException(path: Path) extends InfinityException(s"$path already exists")

case class NotFoundException(path: Path) extends InfinityException(s"$path was not found")
