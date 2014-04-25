/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
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
