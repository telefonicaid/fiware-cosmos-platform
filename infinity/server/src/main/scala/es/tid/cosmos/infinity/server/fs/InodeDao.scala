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

package es.tid.cosmos.infinity.server.fs

import es.tid.cosmos.infinity.common.Path

trait InodeDao[Conn] {

  /** Retrieve the inode associated with the given path.
    *
    * @param path The path whose inode is to be returned.
    * @param c The connection to the persistent store.
    * @return The requested path or None if no inode is found for the given path.
    */
  def lookup(path: Path)(implicit c: Conn): Option[Inode]

  /** List the entries on a directory inode.
    *
    * @param directoryInode The inode to list
    * @param c The connection to the persistent store
    * @return A set of children
    * @throws NoSuchInode if the inode doesn't exist
    */
  def list(directoryInode: DirectoryInode)(implicit c: Conn): Set[ChildInode]

  /** Insert a new object in the store.
    *
    * @param inode The new inode to be inserted in the persistent store.
    * @param c The connection to the persistent store.
    * @throws NoSuchInode if the parent of the given inode is not found.
    */
  def insert(inode: ChildInode)(implicit c: Conn): Unit

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
    */
  def delete(inode: ChildInode)(implicit c: Conn): Unit

  /** Retrieve the path corresponding to given inode.
    *
    * @param inode The inode whose path is to be obtained.
    * @param c The connection to the persistent store
    * @return The path of the given inode
    * @throws NoSuchInode if the given inode doesn't exist
    */
  def pathOf(inode: Inode)(implicit c: Conn): Path
}
