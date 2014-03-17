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

package es.tid.cosmos.infinity.server.auth

import es.tid.cosmos.infinity.server.util.Path

/** An action performed on a Infinity path. */
sealed trait Action {
  val on: Path
}

/** A read action on a Infinity path. */
case class ReadAction(on: Path) extends Action

/** A write action on a Infinity path. */
case class WriteAction(on: Path) extends Action

/** A append action on a Infinity path. */
case class AppendAction(on: Path) extends Action

/** A mkdir action on a Infinity path. */
case class MkdirAction(on: Path) extends Action
