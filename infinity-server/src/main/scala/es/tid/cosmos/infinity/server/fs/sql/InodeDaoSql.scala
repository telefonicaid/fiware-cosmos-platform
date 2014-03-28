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
import scala.annotation.tailrec
import scalaz._

import anorm._

import es.tid.cosmos.infinity.server.authentication.UserProfile
import es.tid.cosmos.infinity.server.authorization.UnixFilePermissions._
import es.tid.cosmos.infinity.server.authorization.FilePermissions
import es.tid.cosmos.infinity.server.fs._
import es.tid.cosmos.infinity.server.util.{RootPath, Path, SubPath}

class InodeDaoSql extends InodeDao[Connection] {

  import Scalaz._

  override def insert(inode: Inode)(implicit c: Connection): Validation[InodeAccessException, Unit] = try {
    SQL(
      """insert into `inode` (`id`, `name`, `directory`, `owner`, `group`, `permissions`, `parent_id`)
        |             values ({id}, {name}, {directory}, {owner}, {group}, {permissions}, {parent_id})
      """.stripMargin)
      .on("id" -> inode.id)
      .on("name" -> inode.name)
      .on("directory" -> inode.directory)
      .on("owner" -> inode.permissions.owner)
      .on("group" -> inode.permissions.group)
      .on("permissions" -> inode.permissions.unix.toString)
      .on("parent_id" -> inode.parentId)
      .executeInsert()
    ().success
  } catch {
    case IntegritySqlException(_) => NoSuchInode(inode.id).failure
  }


  override def update(inode: Inode)(implicit c: Connection): Unit =
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

  override def delete(inode: Inode, user: UserProfile)(implicit c: Connection):
      Validation[InodeAccessException, Unit] = {
    if (!inode.canWrite(user)) return PermissionDenied(pathOf(inode)).failure
    if (inode == RootInode) return PermissionDenied(RootPath).failure
    try {
      SQL("""delete from `inode` where `id` = {id}""")
        .on("id" -> inode.id)
        .execute() match {
          case true => ().success
          case false => NoSuchInode(inode.id).failure
        }
    } catch {
      case IntegritySqlException(_) => DirectoryNonEmpty(pathOf(inode)).failure
    }
  }

  private def findBy(parentId: String, name: String)(implicit c: Connection): Option[Inode] =
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

  override def load(path: Path, user: UserProfile)(implicit c: Connection):
      Validation[InodeAccessException, Inode]  = {

    def find(parent: Inode, name: String) = findBy(parent.id, name) match {
      case Some(inode) if !inode.canExec(user) => throw PermissionDenied(path)
      case Some(inode) => inode
      case None => throw NoSuchFileOrDirectory(path)
    }

    @tailrec
    def pathWalker(pathObj: Seq[String], current: Inode): Inode = pathObj match {
      case Seq(x, xs @ _*) => pathWalker(xs, find(current, x))
      case Seq() => if (current.canRead(user)) current else throw NoSuchFileOrDirectory(path)
    }

    try {
      pathWalker(RootInode.name +: pathElements(path), RootInode).success
    } catch {
      case e: InodeAccessException => e.failure
    }
  }

  private def pathElements(path: Path): Seq[String] = path match {
    case RootPath => Seq(RootInode.name)
    case subPath: SubPath => pathElements(subPath.parent.get) :+ subPath.name
  }

  private def pathOf(inode: Inode)(implicit c: Connection): Path = inode match {
    case RootInode => RootPath
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
