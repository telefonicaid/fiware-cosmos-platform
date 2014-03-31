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

package es.tid.cosmos.infinity.server.authorization

import scalaz.Scalaz

import org.mockito.BDDMockito.given
import org.mockito.Matchers.{eq => the, _}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.server.actions.Action
import es.tid.cosmos.infinity.server.authentication.{UserCredentials, UserProfile}
import es.tid.cosmos.infinity.server.authorization.Authorization.{AuthorizationFailed, Authorized}
import es.tid.cosmos.infinity.server.db.InfinityDataStore
import es.tid.cosmos.infinity.server.fs._
import es.tid.cosmos.infinity.server.util.{RootPath, Path}

class PersistentAuthorizationTest extends FlatSpec with MustMatchers with MockitoSugar {

  import Scalaz._

  class MockDataStore extends InfinityDataStore{
    override type Conn = Unit
    override val inodeDao = mock[InodeDao[Conn]]
    override def withTransaction[A](block: (Conn) => A): A = withConnection(block)
    override def withConnection[A](block: (Conn) => A): A = block(())
  }

  val readable = "444"
  val notReadable = "333"
  val writable = "222"
  val notWritable = "555"

  trait Fixture {
    def userProfile = UserProfile("jsmith", "group", UnixFilePermissions.fromOctal("777"))
    val credentials = UserCredentials("key", "secret")
    val path = Path.absolute("/some/path")
    val parentPath = path.parent.get
    val store = new MockDataStore
    val instance = new PersistentAuthorization(store)

    def givenInodeIsAccessible(
        path: Path,
        permissions: String = "000",
        ownedByUser: Boolean = false): Unit =
      given(store.inodeDao.load(the(path), the(userProfile))(any[store.Conn]))
        .willReturn(Inode(
        id = "id",
        name = "path",
        directory = false,
        permissions = FilePermissions(
          owner = if (ownedByUser) userProfile.username else "otherUser",
          group = "otherGroup",
          unix = UnixFilePermissions.fromOctal(permissions)
        ),
        parentId = "parentId"
      ).success)

    def givenInodeIsNotAccessible(path: Path): Unit =
      given(store.inodeDao.load(the(path), the(userProfile))(any[store.Conn]))
        .willReturn(PermissionDenied(path).failure)

    def mustAuthorize(action: Action): Unit =
      instance.authorize(action, userProfile) must be (Authorized)

    def mustReject(action: Action): Unit = instance.authorize(action, userProfile) must
      be (AuthorizationFailed(PermissionDenied(action.on)))
  }

  "Opening a file" must behave like
    allowingAccessibleInodeWithReadAccess(f => Action.Open(f.path, f.credentials))

  "Getting status of a file" must behave like
    allowingAccessibleInodeWithReadAccess(f => Action.GetFileStatus(f.path, f.credentials))

  "Getting content summary" must behave like
    allowingAccessibleInodeWithReadAccess(f => Action.GetContentSummary(f.path, f.credentials))

  "Getting file checksum" must behave like
    allowingAccessibleInodeWithReadAccess(f => Action.GetFileChecksum(f.path, f.credentials))

  "Listing status" must behave like
    allowingAccessibleInodeWithReadAccess(f => Action.ListStatus(f.path, f.credentials))

  "Getting home directory" must "proceed disregarding the path" in new Fixture {
    mustAuthorize(Action.GetHomeDirectory(RootPath, credentials))
    mustAuthorize(Action.GetHomeDirectory(path, credentials))
  }

  "Creating a file" must "proceed when the parent inode is accessible and writable" in new Fixture {
    givenInodeIsAccessible(parentPath, writable)
    givenInodeIsNotAccessible(path)
    mustAuthorize(Action.Create(path, credentials))
  }

  it must "be rejected when the parent inode is not accessible" in new Fixture {
    givenInodeIsNotAccessible(parentPath)
    givenInodeIsNotAccessible(path)
    mustReject(Action.Create(path, credentials))
  }

  it must "be rejected when the parent inode is not writable" in new Fixture {
    givenInodeIsAccessible(parentPath, notWritable)
    givenInodeIsNotAccessible(path)
    mustReject(Action.Create(path, credentials))
  }

  it must "proceed when the inode is accessible and writable" in new Fixture {
    givenInodeIsAccessible(parentPath)
    givenInodeIsAccessible(path, writable)
    mustAuthorize(Action.Create(path, credentials, overwrite = Some(true)))
  }

  it must "be rejected when inode is not writable" in new Fixture {
    givenInodeIsAccessible(parentPath)
    givenInodeIsAccessible(path, notWritable)
    mustReject(Action.Create(path, credentials))
  }

  "Crating directories" must "proceed when the parent inode exists and is writable" in
    new Fixture {
      givenInodeIsAccessible(parentPath, writable)
      givenInodeIsNotAccessible(path)
      mustAuthorize(Action.Mkdirs(path, credentials))
    }

  it must "be rejected if the parent inode is not writable" in new Fixture {
    givenInodeIsAccessible(parentPath, notWritable)
    givenInodeIsNotAccessible(path)
    mustReject(Action.Mkdirs(path, credentials))
  }

  it must "proceed when multiple paths are to be created (mkdir -p semantics)" in new Fixture {
    givenInodeIsAccessible(parentPath.parent.get, writable)
    givenInodeIsNotAccessible(parentPath)
    givenInodeIsNotAccessible(path)
    mustAuthorize(Action.Mkdirs(path, credentials))
  }

  "Renaming a file" must behave like allowingAccessibleParentInodeWithWriteAccess { f =>
    val newParentPath = Path.absolute("/new/path")
    val destination = (newParentPath / "destination").toString
    f.givenInodeIsAccessible(newParentPath, writable)
    f.givenInodeIsAccessible(f.path, readable)
    Action.Rename(f.path, f.credentials, destination)
  }

  it must "be rejected if destination path is malformed" in new Fixture {
    val newParentPath = Path.absolute("/new/path")
    val destination = "malformed \\_++++:"
    givenInodeIsAccessible(parentPath, writable)
    givenInodeIsAccessible(newParentPath, writable)
    givenInodeIsAccessible(path, readable)
    mustReject(Action.Rename(path, credentials, destination))
  }

  it must "be rejected if the new parent node is not accessible" in new Fixture {
    val newParentPath = Path.absolute("/new/path")
    val destination = (newParentPath / "destination").toString
    givenInodeIsAccessible(parentPath, writable)
    givenInodeIsNotAccessible(newParentPath)
    givenInodeIsAccessible(path, readable)
    mustReject(Action.Rename(path, credentials, destination))
  }

  it must "be rejected if the new parent node is not writable" in new Fixture {
    val newParentPath = Path.absolute("/new/path")
    val destination = (newParentPath / "destination").toString
    givenInodeIsAccessible(parentPath, writable)
    givenInodeIsAccessible(newParentPath, notWritable)
    givenInodeIsAccessible(path, readable)
    mustReject(Action.Rename(path, credentials, destination))
  }

  "Getting a delegation token" must behave like
    alwaysRejected(f => Action.GetDelegationToken(f.path, f.credentials, "user"))
  "Renewing a delegation token" must behave like
    alwaysRejected(f => Action.RenewDelegationToken(f.path, f.credentials, "token"))
  "Cancelling a delegation token" must behave like
    alwaysRejected(f => Action.CancelDelegationToken(f.path, f.credentials, "token"))

  "Setting replication factor" must behave like
    allowingAccessibleInodeWithWriteAccess(f => Action.SetReplication(f.path, f.credentials))

  "Setting the owner of a path" must "proceed for the super user" in new Fixture {
    override def userProfile = super.userProfile.copy(superuser = true)
    mustAuthorize(Action.SetOwner(path, credentials))
  }

  it must "fail otherwise" in new Fixture {
    mustReject(Action.SetOwner(path, credentials))
  }

  "Setting permissions" must behave like  allowingAccessibleParentInodeWithWriteAccess { f =>
    f.givenInodeIsAccessible(f.path, ownedByUser = true)
    Action.SetPermission(f.path, f.credentials)
  }

  it must "be rejected when path is not accessible" in new Fixture {
    givenInodeIsAccessible(parentPath)
    givenInodeIsNotAccessible(path)
    mustReject(Action.SetPermission(path, credentials))
  }

  it must "be rejected when path is not owned by the requester" in new Fixture {
    givenInodeIsAccessible(parentPath)
    givenInodeIsAccessible(path)
    mustReject(Action.SetPermission(path, credentials))
  }

  "Setting times" must behave like
    allowingAccessibleInodeWithWriteAccess(f => Action.SetTimes(f.path, f.credentials))

  "Appending to a file" must behave like
    allowingAccessibleInodeWithWriteAccess(f => Action.Append(f.path, f.credentials))

  "Deleting a file" must behave like
    allowingAccessibleParentInodeWithWriteAccess(f => Action.Delete(f.path, f.credentials))

  def allowingAccessibleInodeWithReadAccess(buildAction: Fixture => Action): Unit = {

    it must "proceed when inode is accessible and can be read" in new Fixture {
      givenInodeIsAccessible(path, readable)
      mustAuthorize(buildAction(this))
    }

    rejectingNonAccessibleInode(buildAction)
    rejectingWhenLackingReadPermissions(buildAction)
  }

  def allowingAccessibleInodeWithWriteAccess(buildAction: Fixture => Action): Unit = {

    it must "proceed when inode is accessible and can be written on" in new Fixture {
      givenInodeIsAccessible(path, writable)
      mustAuthorize(buildAction(this))
    }

    it must "be rejected when lacking write permissions" in new Fixture {
      givenInodeIsAccessible(path, notWritable)
      mustReject(buildAction(this))
    }

    rejectingNonAccessibleInode(buildAction)
  }

  def allowingAccessibleParentInodeWithWriteAccess(buildAction: Fixture => Action): Unit = {

    it must "proceed if the parent inode is accessible and writable" in new Fixture {
      givenInodeIsAccessible(parentPath, writable)
      mustAuthorize(buildAction(this))
    }

    it must "be rejected if parent inode is not accessible" in new Fixture {
      givenInodeIsNotAccessible(parentPath)
      mustReject(buildAction(this))
    }

    it must "be rejected if parent node is not writable" in new Fixture {
      givenInodeIsAccessible(parentPath, notWritable)
      mustReject(buildAction(this))
    }
  }

  def rejectingNonAccessibleInode(buildAction: Fixture => Action): Unit = {
    it must "be rejected when inode is not accessible" in new Fixture {
      givenInodeIsNotAccessible(path)
      mustReject(buildAction(this))
    }
  }

  def rejectingWhenLackingReadPermissions(buildAction: Fixture => Action): Unit = {
    it must "be rejected when lacking read permissions" in new Fixture {
      givenInodeIsAccessible(path, notReadable)
      mustReject(buildAction(this))
    }
  }

  def alwaysRejected(buildAction: Fixture => Action): Unit = {
    it must "be rejected" in new Fixture {
      mustReject(buildAction(this))
    }
  }
}
