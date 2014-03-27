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

import scalaz._

import com.imageworks.migration._
import org.scalatest.{BeforeAndAfterAll, FlatSpec}
import org.scalatest.matchers.MustMatchers
import scalikejdbc.ConnectionPool

import es.tid.cosmos.infinity.server.authorization.FilePermissions
import es.tid.cosmos.infinity.server.authorization.UnixFilePermissions._
import es.tid.cosmos.infinity.server.db.sql.migrations.Migrate_1_InitialVersion
import es.tid.cosmos.infinity.server.fs.{RootInode, MysqlH2DatabaseAdapter, SomeUserProfiles}

class InfinityDataStoreSqlTest extends FlatSpec with MustMatchers with BeforeAndAfterAll
    with SomeUserProfiles {

  override protected def beforeAll(): Unit = {

    java.sql.DriverManager.registerDriver(new org.h2.Driver)
    ConnectionPool.singleton("jdbc:h2:mem:InodeDaoTest;MODE=MYSQL", "", "")

    val migrator = new Migrator(
      new ConnectionBuilder(ConnectionPool.dataSource()),
      new MysqlH2DatabaseAdapter(schemaNameOpt = None))

    migrator.migrate(
      InstallAllMigrations,
      packageName = classOf[Migrate_1_InitialVersion].getPackage.getName,
      searchSubPackages = false)

  }

  override protected def afterAll(): Unit = ConnectionPool.closeAll()

  lazy val store = new InfinityDataStoreSql(ConnectionPool.dataSource())

  "InodeDao" must "be able to find root directory" in  {
    store.withConnection { implicit conn =>
      val root = store.inodeDao.load("/", bob)
      root map (_.name) must be (Success("/"))
    }
  }

  it must "reject / deletion even from super-user" in {
    store.withConnection { implicit conn =>
      for {
        root <- store.inodeDao.load("/", hdfs)
      } yield store.inodeDao.delete(root, hdfs) must be ('failure)
    }
  }

  it must "allow creation of /users/bob" in {
    store.withTransaction { implicit conn =>
      val usersDir = RootInode.create("users", true, hdfs)
      store.inodeDao.insert(usersDir)
      val bobDir = usersDir.create("bob", true, bob,
          FilePermissions("bob", "staff", fromOctal("750")))
      store.inodeDao.insert(bobDir)
    }
  }

  it must "allow get /users/bob by bob user" in {
    store.withConnection { implicit conn =>
      val getBobDir = store.inodeDao.load("/users/bob", bob)
      getBobDir map (_.name) must be (Success("bob"))
    }
  }

  it must "allow get /users/bob by staff group" in {
    store.withConnection { implicit conn =>
      val getBobDir = store.inodeDao.load("/users/bob", alice)
      getBobDir map (_.name) must be (Success("bob"))
    }
  }

  it must "reject get /users/bob by other user" in {
    store.withConnection { implicit conn =>
      store.inodeDao.load("/users/bob", john) must be ('failure)
    }
  }

  it must "reject creation of inode with non-persisted parent" in {
    store.withTransaction { implicit conn =>
      val dataDir = RootInode.create("data", true, hdfs)
      // note: dataDir not inserted
      val bobDir = dataDir.create("bob-data", true, bob,
          FilePermissions("bob", "staff", fromOctal("750")))
      store.inodeDao.insert(bobDir) must be ('failure)
    }
  }

  it must "allow renaming of /users/bob by bob" in {
    store.withConnection { implicit conn =>
      for {
        bobDir <- store.inodeDao.load("/users/bob", bob)
        bobRenamed = bobDir.update(name = Some("super-bob"), user = bob)
      } yield store.inodeDao.update(bobRenamed)
    }
  }

  it must "allow move /users/super-bob to /super-bob by bob" in {
    store.withConnection { implicit conn =>
      for {
        root <- store.inodeDao.load("/", bob)
        bobDir <- store.inodeDao.load("/users/super-bob", bob)
        movedBob = bobDir.update(parent = Some(root.id), user = bob)
      } yield store.inodeDao.update(movedBob)
    }
  }

  it must "allow deletion /super-bob by bob" in {
    store.withTransaction { implicit conn =>
      for {
        bobDir <- store.inodeDao.load("/super-bob", bob)
      } yield store.inodeDao.delete(bobDir, bob)
    }
    store.withConnection { implicit conn =>
      store.inodeDao.load("/super-bob", bob) must be ('failure)
    }
  }

  it must "reject open a file with no read permissions" in {
    store.withConnection { implicit conn =>
      for {
        parent <- store.inodeDao.load("/", bob)
      } yield {
        store.inodeDao.insert(parent.create("bob-file.txt", false, bob,
          FilePermissions(bob.username, bob.group, fromOctal("700"))))
      }
    }
    store.withConnection { implicit conn =>
      store.inodeDao.load("/bob-file.txt", alice) must be ('failure)
    }
  }

  it must "reject delete a file with no write permissions" in {
    store.withConnection { implicit conn =>
      for {
        parent <- store.inodeDao.load("/", bob)
      } yield {
        store.inodeDao.insert(parent.create("private-file.txt", false, bob,
          FilePermissions(bob.username, bob.group, fromOctal("644"))))
      }
    }
    store.withConnection { implicit conn =>
      for {
        file <- store.inodeDao.load("/private-file.txt", alice)
      } yield store.inodeDao.delete(file, alice) must be ('failure)
    }
  }

  it must "reject delete a non-empty directory" in {
    store.withTransaction { implicit conn =>
      for {
        root <- store.inodeDao.load("/", bob)
      } yield {
        val dataDir = root.create("data", true, bob,
          FilePermissions(bob.username, bob.group, fromOctal("700")))
        store.inodeDao.insert(dataDir)
        store.inodeDao.insert(dataDir.create("file", false, bob))
      }
    }
    store.withConnection { implicit conn =>
      for {
        file <- store.inodeDao.load("/data", bob)
      } yield store.inodeDao.delete(file, bob) must be ('failure)
    }
  }
}
