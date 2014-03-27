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

import scalaz.Validation

import es.tid.cosmos.infinity.server.authentication.UserProfile

trait InodeDao[Conn] {

  def insert(parent: Inode)(implicit c: Conn): Validation[InodeAccessException, Unit]

  def update(inode: Inode)(implicit c: Conn): Unit

  def delete(inode: Inode, user: UserProfile)(implicit c: Conn):
    Validation[InodeAccessException, Unit]

  /** Find recursively walking from Root directory all the path.
    *
    * @throws InodeAccessException if path does not exists or cannot be reached
    *                              due to permission restrictions.
    */
  def load(path: String, user: UserProfile)(implicit c: Conn):
    Validation[InodeAccessException, Inode]

}
