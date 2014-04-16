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

package es.tid.cosmos.infinity.server.fs.sql

import scala.util.Random

import com.imageworks.migration._
import org.scalatest.{BeforeAndAfter, FlatSpec}
import org.scalatest.matchers.MustMatchers
import scalikejdbc.ConnectionPool

import es.tid.cosmos.infinity.server.authentication.UserProfile
import es.tid.cosmos.infinity.server.db.sql.migrations.Migrate_1_InitialVersion
import es.tid.cosmos.infinity.server.fs._
import es.tid.cosmos.infinity.server.permissions.PermissionsMask._
import es.tid.cosmos.infinity.server.util.{Path, RootPath, SubPath}

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
      root.get.id must be (RootInode.Id)
    }
  }

  it must "find an existing path" in new Fixture {
    store.withConnection { implicit conn =>
      val (path, _) = givenRandomFile()
      val inode = store.inodeDao.lookup(path)
      inode must be ('defined)
      store.inodeDao.pathOf(inode.get) must be (path)
    }
  }

  it must "not find a non existing path" in new Fixture {
    store.withConnection { implicit conn =>
      store.inodeDao.lookup(Path.absolute("/non/existing/path"))
    } must be ('empty)
  }

  "Directory listing" must "list root children" in new Fixture {
    val (path1, _) = givenRandomDirectory()
    val (path2, _) = givenRandomDirectory()
    store.withConnection { implicit conn =>
      store.inodeDao.list(rootInode)
    } must have size 2
  }

  it must "list directory children" in new Fixture {
    val (basePath, baseInode) = givenRandomDirectory()
    val inode1 = baseInode.newFile("file1")
    val inode2 = baseInode.newFile("file2")

    store.withConnection { implicit conn =>
      store.inodeDao.insert(inode1)
      store.inodeDao.insert(inode2)
      store.inodeDao.list(baseInode)
    } must be (Set(inode1, inode2))
  }

  it must "fail on unexisting directory" in new Fixture {
    val (_, baseInode) = givenRandomDirectory(exists = false)

    store.withConnection { implicit conn =>
      evaluating { store.inodeDao.list(baseInode) } must produce [NoSuchInode]
    }
  }

  "Inode creation" must "allow creation of path" in new Fixture {
    val (path, inode) = givenRandomDirectory(exists = false)
    store.withTransaction { implicit conn =>
      store.inodeDao.insert(inode)
      store.inodeDao.lookup(path) must be (Some(inode))
    }
  }

  it must "reject creation of inode with unknown parent inode" in new Fixture {
    val (_, dataDir) = givenRandomDirectory(exists = false)
    store.withTransaction { implicit conn =>
      val bobDir = dataDir.newDirectory(
        "bob-data", FilePermissions("bob", "staff", fromOctal("750")))
      evaluating { store.inodeDao.insert(bobDir) } must produce [NoSuchInode]
    }
  }

  it must "fail when creating a sibling path with the same name" in new Fixture {
    val (path, inode) = givenRandomFile()
    store.withTransaction { implicit conn =>
      val Some(parentInode: DirectoryInode) = store.inodeDao.lookup(path.parent.get)
      evaluating {
        store.inodeDao.insert(parentInode.newFile(inode.name))
      } must produce [PathAlreadyExists]
    }
  }

  "Deleting inodes" must "be rejected on a non-empty directory" in new Fixture {
    val (filePath, _) = givenRandomFile()
    store.withTransaction { implicit conn =>
      val Some(inode: SubDirectoryInode) = store.inodeDao.lookup(filePath.parent.get)
      evaluating { store.inodeDao.delete(inode) } must produce [DirectoryNonEmpty]
    }
  }

  it must "be rejected on a non-existing inode" in new Fixture {
    val (_, inode) = givenRandomFile(exists = false)
    store.withConnection { implicit conn =>
      evaluating { store.inodeDao.delete(inode) } must produce [NoSuchInode]
    }
  }

  "Updating inodes" must "allow changing inode name, owner, group, permissions and parent" in
    new Fixture {
      val (_, inode) = givenRandomFile()
      val (newDirPath, newDir) = givenRandomDirectory()
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
    val (_, inode) = givenRandomFile()
    val updatedInode = SubDirectoryInode(inode.id, inode.parentId, inode.name, inode.permissions)
    store.withTransaction { implicit conn =>
      evaluating {
        store.inodeDao.update(updatedInode)
      } must produce [InvalidOperation]
    }
  }

  it must "reject changing non-existing inodes" in new Fixture {
    val (_, inode) = givenRandomFile(exists = false)
    store.withTransaction { implicit conn =>
      evaluating {
        store.inodeDao.update(inode)
      } must produce [NoSuchInode]
    }
  }

  it must "reject changing parent to non-directory inode" in new Fixture {
    val (_, inode) = givenRandomFile()
    val (_, newFile) = givenRandomFile()
    val updatedInode = inode.copy(parentId = newFile.id)
    store.withTransaction { implicit conn =>
      evaluating {
        store.inodeDao.update(updatedInode)
      } must produce [InvalidOperation]
    }
  }

  it must "reject changing parent to non-existing inode" in new Fixture {
    val (_, inode) = givenRandomFile()
    val (_, unexistingDir) = givenRandomDirectory(exists = false)
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
    def rootInode: RootInode = store.withConnection { implicit conn =>
      store.inodeDao.lookup(RootPath).get.asInstanceOf[RootInode]
    }
    val directoryPermissions = FilePermissions("saruman", "istari", fromOctal("755"))
    val filePermissions = FilePermissions("saruman", "istari", fromOctal("644"))

    def givenRandomDirectory(exists: Boolean = true): (SubPath, SubDirectoryInode) = {
      val (parentPath, parentInode: DirectoryInode) =
        if (Random.nextBoolean()) (RootPath, rootInode) else givenRandomDirectory()
      val name = randomName()
      val inode = parentInode.newDirectory(name, directoryPermissions)
      if (exists) {
        store.withTransaction(implicit c => store.inodeDao.insert(inode))
      }
      (parentPath / name, inode)
    }

    def givenRandomFile(exists: Boolean = true): (SubPath, FileInode) = {
      val (parentPath, parentInode) = givenRandomDirectory(exists = true)
      val name = randomName()
      val inode = parentInode.newFile(name, filePermissions)
      if (exists) {
        store.withTransaction(implicit c => store.inodeDao.insert(inode))
      }
      (parentPath / name, inode)
    }

    private def randomName(): String = Random.alphanumeric.take(16).mkString
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
