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

import scala.util.Random

import com.imageworks.migration._
import org.scalatest.{BeforeAndAfter, FlatSpec}
import org.scalatest.matchers.MustMatchers
import scalikejdbc.ConnectionPool

import es.tid.cosmos.infinity.server.authentication.UserProfile
import es.tid.cosmos.infinity.server.authorization.{FilePermissions, UnixFilePermissions}
import es.tid.cosmos.infinity.server.authorization.UnixFilePermissions._
import es.tid.cosmos.infinity.server.db.sql.migrations.Migrate_1_InitialVersion
import es.tid.cosmos.infinity.server.fs._
import es.tid.cosmos.infinity.server.util.{Path, RootPath}

class InfinityDataStoreSqlTest extends FlatSpec with MustMatchers with BeforeAndAfter {

  before {
    createDatabase()
    populateDatabase()
  }

  after { shutdownDatabase() }

  "Inode lookup" must "find the root directory" in new Fixture {
    store.withConnection { implicit conn =>
      val root = store.inodeDao.lookup(RootPath)
      root must be ('defined)
      root.get.name must be ("/")
    }
  }

  it must "find an existing path" in new Fixture {
    store.withConnection { implicit conn =>
      val (path, _) = givenRandomPath()
      val inode = store.inodeDao.lookup(path)
      inode must be ('defined)
      s"/${inode.get.name}" must be (path.toString)
    }
  }

  it must "not find a non existing path" in new Fixture {
    store.withConnection { implicit conn =>
      store.inodeDao.lookup(Path.absolute("/non/existing/path"))
    } must be ('empty)
  }

  "Inode creation" must "allow creation of path" in new Fixture {
    val (path, inode) = givenPath("name", isDirectory = true, exists = false)
    store.withTransaction { implicit conn =>
      store.inodeDao.insert(inode)
      store.inodeDao.lookup(path) must be (Some(inode))
    }
  }

  it must "reject creation of inode with unknown parent inode" in new Fixture {
    val (_, dataDir) = givenPath("data", isDirectory = true, exists = false)
    store.withTransaction { implicit conn =>
      val bobDir = dataDir.newChild("bob-data", isDirectory = true, user = sampleUser,
        FilePermissions("bob", "staff", fromOctal("750")))
      evaluating { store.inodeDao.insert(bobDir) } must produce [NoSuchInode]
    }
  }

  "Deleting inodes" must "be rejected on root path" in new Fixture {
    store.withConnection { implicit conn =>
      evaluating { store.inodeDao.delete(rootInode) } must produce [InvalidOperation]
    }
  }

  it must "be rejected on a non-empty directory" in new Fixture {
    val (dataDir, dataInode) = givenRandomPath(isDirectory = true)
    store.withTransaction { implicit conn =>
      store.inodeDao.insert(dataInode.newChild("file", isDirectory = false, user = sampleUser))
      evaluating { store.inodeDao.delete(dataInode) } must produce [DirectoryNonEmpty]
    }
  }

  it must "be rejected on a non-existing inode" in new Fixture {
    val inode = rootInode.newChild("bob-data", isDirectory = true, user = sampleUser,
      FilePermissions("bob", "staff", fromOctal("750")))
    store.withConnection { implicit conn =>
      evaluating { store.inodeDao.delete(inode) } must produce [NoSuchInode]
    }
  }

  "Updating inodes" must "allow changing inode name, owner, group, permissions and parent" in
    new Fixture {
      val (_, inode) = givenRandomPath()
      val (newDirPath, newDir) = givenRandomPath(isDirectory = true)
      val updatedInode = inode.copy(
        name = "new_name",
        permissions = FilePermissions(
          owner = "other",
          group = "other_group",
          unix = fromOctal("666")
        ),
        parentId = newDir.id
      )
      store.withTransaction { implicit conn =>
        store.inodeDao.update(updatedInode)
        store.inodeDao.lookup(newDirPath / "new_name") must be (Some(updatedInode))
      }
    }

  it must "allow changing root inode owner, group and permissions" in new Fixture {
    val updatedInode = rootInode.copy(
      permissions = FilePermissions(
        owner = "other",
        group = "other_group",
        unix = fromOctal("666")
      )
    )
    store.withTransaction { implicit conn =>
      store.inodeDao.update(updatedInode)
      store.inodeDao.lookup(RootPath) must be (Some(updatedInode))
    }
  }

  it must "reject changing inode type" in new Fixture {
    val (_, inode) = givenRandomPath()
    val updatedInode = inode.copy(isDirectory = true)
    store.withTransaction { implicit conn =>
      evaluating {
        store.inodeDao.update(updatedInode)
      } must produce [InvalidOperation]
    }
  }

  it must "reject changing root inode name" in new Fixture {
    val updatedRoot = rootInode.copy(name = "my_new_root")
    store.withTransaction { implicit conn =>
      evaluating {
        store.inodeDao.update(updatedRoot)
      } must produce [InvalidOperation]
    }
  }

  it must "reject changing non-existing inodes" in new Fixture {
    val (_, inode) = givenRandomPath()
    val updatedInode = inode.copy(id = "unknown_id")
    store.withTransaction { implicit conn =>
      evaluating {
        store.inodeDao.update(updatedInode)
      } must produce [NoSuchInode]
    }
  }

  it must "reject changing parent on root inode" in new Fixture {
    val (_, newDir) = givenRandomPath(isDirectory = true)
    val updatedRoot = rootInode.copy(parentId = newDir.id)
    store.withTransaction { implicit conn =>
      evaluating {
        store.inodeDao.update(updatedRoot)
      } must produce [InvalidOperation]
    }
  }

  it must "reject changing parent to non-directory inode" in new Fixture {
    val (_, inode) = givenRandomPath()
    val (_, newFile) = givenRandomPath()
    val updatedInode = inode.copy(parentId = newFile.id)
    store.withTransaction { implicit conn =>
      evaluating {
        store.inodeDao.update(updatedInode)
      } must produce [InvalidOperation]
    }
  }

  it must "reject changing parent to non-existing inode" in new Fixture {
    val (_, inode) = givenRandomPath()
    val (_, unexistingDir) = givenRandomPath(isDirectory = true, exists = false)
    val updatedInode = inode.copy(parentId = unexistingDir.id)
    store.withTransaction { implicit conn =>
      evaluating {
        store.inodeDao.update(updatedInode)
      } must produce [NoSuchInode]
    }
  }

  trait Fixture {
    val sampleUser = UserProfile("bob", "staff")
    val store = new InfinityDataStoreSql(ConnectionPool.dataSource())
    def rootInode = store.withConnection { implicit conn =>
      store.inodeDao.lookup(RootPath).get
    }

    def givenRandomPath(isDirectory: Boolean = false, exists: Boolean = true) =
      givenPath(Random.alphanumeric.take(16).mkString, isDirectory, exists)

    def givenPath(
                   inodeName: String, isDirectory: Boolean = false, exists: Boolean = true): (Path, Inode) = {
      val childInode = rootInode.newChild(
        name = inodeName,
        isDirectory = isDirectory,
        user = UserProfile("saruman", "istari", UnixFilePermissions.fromOctal("755"))
      )
      if (exists) {
        store.withConnection { implicit conn => store.inodeDao.insert(childInode) }
      }
      (Path.absolute(s"/$inodeName"), childInode)
    }
  }

  private def createDatabase(): Unit = {
    java.sql.DriverManager.registerDriver(new org.h2.Driver)
    ConnectionPool.singleton("jdbc:h2:mem:InodeDaoTest;MODE=MYSQL", "", "")
  }

  private def populateDatabase(): Unit = {
    val migrator = new Migrator(
      new ConnectionBuilder(ConnectionPool.dataSource()),
      new MysqlH2DatabaseAdapter(schemaNameOpt = None))

    migrator.migrate(
      InstallAllMigrations,
      packageName = classOf[Migrate_1_InitialVersion].getPackage.getName,
      searchSubPackages = false)
  }

  private def shutdownDatabase(): Unit = ConnectionPool.closeAll()
}
