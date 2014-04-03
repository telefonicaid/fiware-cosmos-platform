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

trait InodeDao[Conn] {

  /** Retrieve the inode associated with the given path.
    *
    * @param path The path whose inode is to be returned.
    * @param c The connection to the persistent store.
    * @return The requested path or None if no inode is found for the given path.
    */
  def lookup(path: Path)(implicit c: Conn): Option[Inode]

  /** Insert a new inode object in the store.
    *
    * The inode object shall be obtained as result of [[Inode.newChild()]] operation.
    *
    * @param inode The new inode to be inserted in the persistent store.
    * @param c The connection to the persistent store.
    * @throws NoSuchInode if the parent of the given inode is not found.
    */
  def insert(inode: Inode)(implicit c: Conn): Unit

  /** Update inode in the store.
    *
    * A preexisting inode with the same id is replaced on the store.
    *
    * @param inode  Replacement for the inode
    * @param c      The connection to the persistent store
    * @throws NoSuchInode if the inode doesn't exist
    * @throws InvalidOperation if the replacement changes the inode type or the name in
    *                          case of root inode
    */
  def update(inode: Inode)(implicit c: Conn): Unit

  /** Delete the given inode from the store.
    *
    * @param inode The inode to be removed.
    * @param c The connection to the persistent store
    * @throws NoSuchInode if the given inode doesn't exist
    * @throws DirectoryNonEmpty if the given inode corresponds to a non-empty directory
    * @throws InvalidOperation if the given inode corresponds to root inode
    */
  def delete(inode: Inode)(implicit c: Conn): Unit
}
