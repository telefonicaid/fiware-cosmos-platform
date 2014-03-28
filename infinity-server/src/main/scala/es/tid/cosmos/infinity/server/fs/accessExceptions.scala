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

package es.tid.cosmos.infinity.server.fs

import es.tid.cosmos.infinity.server.util.Path

sealed trait InodeAccessException extends Exception

case class PermissionDenied(path: Path) extends InodeAccessException
case class NoSuchInode(id: String) extends InodeAccessException
case class NoSuchFileOrDirectory(path: Path) extends InodeAccessException
case class DirectoryNonEmpty(path: Path) extends InodeAccessException
