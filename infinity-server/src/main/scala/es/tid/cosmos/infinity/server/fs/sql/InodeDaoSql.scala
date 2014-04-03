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

import es.tid.cosmos.infinity.server.authorization.FilePermissions
import es.tid.cosmos.infinity.server.authorization.UnixFilePermissions._
import es.tid.cosmos.infinity.server.fs._
import es.tid.cosmos.infinity.server.util.{Path, RootPath, SubPath}

class InodeDaoSql extends InodeDao[Connection] {


  override def lookup(path: Path)(implicit c: Connection): Option[Inode] = path match {
    case RootPath => lookup(RootInode.Id, RootInode.Name)
    case SubPath(parent, name) => for {
      parentInode <- lookup(parent)
      inode <- lookup(parentInode.id, name)
    } yield inode
  }

  override def list(directoryInode: DirectoryInode)(implicit c: Connection): Set[ChildInode] = {
    if (findBy(directoryInode.id).isEmpty) throw NoSuchInode(directoryInode.id)
    SQL(
      """select `id`, `name`, `directory`, `owner`, `group`, `permissions`, `parent_id`
        | from `inode` where `parent_id` = {id}
      """.stripMargin)
      .on("id" -> directoryInode.id)
      .apply()
      .collect(asChildInode).toSet
  }

  override def insert(inode: ChildInode)(implicit c: Connection): Unit = try {
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

  override def update(inode: Inode)(implicit c: Connection): Unit = inode match {
    case RootInode(permissions) => updateRootPermissions(permissions)
    case childInode: ChildInode => updateChildInode(childInode)
  }

  private def updateRootPermissions(permissions: FilePermissions)(implicit c: Connection): Unit =
    SQL("""update `inode` set `owner` = {owner}, `group` = {group}, `permissions` = {permissions}
          | where `id` = {id}
        """.stripMargin)
      .on("id" -> RootInode.Id)
      .on("owner" -> permissions.owner)
      .on("group" -> permissions.group)
      .on("permissions" -> permissions.unix.toString)
      .executeUpdate()

  private def updateChildInode(inode: ChildInode)(implicit c: Connection): Unit = {
    val prevInode = findBy(inode.id) match {
      case Some(childInode: ChildInode) => childInode
      case _ => throw NoSuchInode(inode.id)
    }
    val hasSameType = prevInode.isDirectory == inode.isDirectory
    val newParentIsDirectory = (prevInode.parentId == inode.parentId) ||
      findBy(inode.parentId).getOrElse(throw NoSuchInode(inode.parentId)).isDirectory
    val isValidUpdate: Boolean = hasSameType && newParentIsDirectory

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

  override def delete(inode: ChildInode)(implicit c: Connection): Unit = {
    val success = try {
      SQL( """delete from `inode` where `id` = {id}""")
        .on("id" -> inode.id)
        .execute()
    } catch {
      case IntegritySqlException(_) => throw DirectoryNonEmpty(pathOf(inode))
    }
    if (!success) throw NoSuchInode(inode.id)
  }

  override def pathOf(inode: Inode)(implicit c: Connection): Path = inode match {
    case RootInode(_) => RootPath
    case child : ChildInode =>
      val parentNode = findBy(child.parentId).getOrElse(throw new IllegalStateException(
        s"Cannot load inode '${child.parentId}' so '${child.id}' is an orphan inode."
      ))
      pathOf(parentNode) / child.name
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

  private val asRootInode: PartialFunction[Row, RootInode] = {
    case Row(RootInode.Id, _, _, owner: String, group: String, permissions: String, _) =>
      RootInode(FilePermissions(owner, group, fromOctal(permissions)))
  }

  private val asChildInode: PartialFunction[Row, ChildInode] = {
    case Row(id: String, name: String, directory: Boolean, owner: String,
    group: String, permissions: String, parentId: String) if id != RootInode.Id =>
      val perm = FilePermissions(owner, group, fromOctal(permissions))
      if (directory) SubDirectoryInode(id, parentId, name, perm)
      else FileInode(id, parentId, name, perm)
  }

  private val asInode: PartialFunction[Row, Inode] = asRootInode orElse asChildInode
}
