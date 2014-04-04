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

package es.tid.cosmos.infinity.server.fs.sql

import java.sql.Connection

import anorm._

import es.tid.cosmos.infinity.server.authorization.UnixFilePermissions._
import es.tid.cosmos.infinity.server.authorization.FilePermissions
import es.tid.cosmos.infinity.server.fs._
import es.tid.cosmos.infinity.server.util.{RootPath, Path, SubPath}

class InodeDaoSql extends InodeDao[Connection] {


  override def lookup(path: Path)(implicit c: Connection): Option[Inode] = path match {
    case RootPath => lookup(Inode.RootId, Inode.RootName)
    case SubPath(parent, name) => for {
      parentInode <- lookup(parent)
      inode <- lookup(parentInode.id, name)
    } yield inode
  }

  override def insert(inode: Inode)(implicit c: Connection): Unit = try {
    SQL(
      """insert into `inode` (`id`, `name`, `directory`, `owner`, `group`, `permissions`, `parent_id`)
        |             values ({id}, {name}, {directory}, {owner}, {group}, {permissions}, {parent_id})
      """.stripMargin)
      .on("id" -> inode.id)
      .on("name" -> inode.name)
      .on("directory" -> inode.isDirectory)
      .on("owner" -> inode.permissions.owner)
      .on("group" -> inode.permissions.group)
      .on("permissions" -> inode.permissions.unix.toString)
      .on("parent_id" -> inode.parentId)
      .executeInsert()
  } catch {
    case IntegritySqlException(_) =>
      // TODO: pass inner exception to NoSuchNode as cause
      throw NoSuchInode(inode.id)
  }

  override def update(inode: Inode)(implicit c: Connection): Unit = {
    val prevInode = findBy(inode.id).getOrElse(throw NoSuchInode(inode.id))
    val isValidRoot = inode.name == Inode.RootName && inode.parentId == Inode.RootId
    val isSameType = prevInode.isDirectory == inode.isDirectory
    val newParentIsDirectory = (prevInode.parentId == inode.parentId) ||
      findBy(inode.parentId).getOrElse(throw NoSuchInode(inode.parentId)).isDirectory
    val isValidUpdate: Boolean =
      isSameType && (!prevInode.isRoot || isValidRoot) && newParentIsDirectory

    if (!isValidUpdate) throw InvalidOperation(pathOf(prevInode))
    SQL(
      """update `inode`
        | set `name` = {name}, `owner` = {owner}, `group` = {group},
        |     `permissions` = {permissions}, `parent_id` = {parent_id}
        | where `id` = {id}
      """.stripMargin)
      .on("name" -> inode.name)
      .on("owner" -> inode.permissions.owner)
      .on("group" -> inode.permissions.group)
      .on("permissions" -> inode.permissions.unix.toString)
      .on("parent_id" -> inode.parentId)
      .on("id" -> inode.id)
      .executeUpdate()
  }

  override def delete(inode: Inode)(implicit c: Connection): Unit = {
    if (inode.isRoot) throw InvalidOperation(RootPath)
    val success = try {
      SQL( """delete from `inode` where `id` = {id}""")
        .on("id" -> inode.id)
        .execute()
    } catch {
      case IntegritySqlException(_) => throw DirectoryNonEmpty(pathOf(inode))
    }
    if (!success) throw NoSuchInode(inode.id)
  }

  private def lookup(parentId: String, name: String)(implicit c: Connection): Option[Inode] =
    SQL(
      """select `id`, `name`, `directory`, `owner`, `group`, `permissions`, `parent_id`
        | from `inode` where `parent_id` = {parentId} and `name` = {name}
      """.stripMargin)
      .on("parentId" -> parentId)
      .on("name" -> name)
      .apply()
      .collectFirst(asInode)

  private def findBy(id: String)(implicit c: Connection): Option[Inode] =
    SQL(
      """select `id`, `name`, `directory`, `owner`, `group`, `permissions`, `parent_id`
        | from `inode` where `id` = {id}
      """.stripMargin)
      .on("id" -> id)
      .apply()
      .collectFirst(asInode)

  private def pathOf(inode: Inode)(implicit c: Connection): Path = inode match {
    case Inode(Inode.RootId, _, _, _, _) => RootPath
    case Inode(nodeId, name, _, _, parentId) =>
      val parentNode = findBy(parentId).getOrElse(throw new IllegalStateException(
        s"Cannot load inode '$parentId' so '$nodeId' is an orphan inode."
      ))
      pathOf(parentNode) / name
  }

  private val asInode: PartialFunction[Row, Inode] = {
    case Row(id: String, name: String, directory: Boolean, owner: String,
    group: String, permissions: String, parentId: String) =>
      new Inode(id, name, directory, FilePermissions(owner, group, fromOctal(permissions)), parentId)
  }
}
