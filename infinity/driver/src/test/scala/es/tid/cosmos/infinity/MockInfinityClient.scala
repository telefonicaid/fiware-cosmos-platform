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

package es.tid.cosmos.infinity

import java.net.URL
import java.util.Date
import scala.concurrent.{Future, Promise}

import org.mockito.BDDMockito.{BDDMyOngoingStubbing, given}
import org.mockito.Matchers.{any, anyBoolean, anyInt, eq => the}
import org.mockito.Mockito.{never, verify}
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.client.InfinityClient
import es.tid.cosmos.infinity.common.fs._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import java.io.{ByteArrayOutputStream, OutputStreamWriter}

class MockInfinityClient extends MockitoSugar {

  val value = mock[InfinityClient]

  val defaultMetadataUrl = new URL("https://metadata/some/path")
  val defaultTime = new Date(449964000000L)
  val defaultMask = PermissionsMask.fromOctal("755")
  val defaultFailure = new IllegalStateException("forced failure")

  def givenNonExistingPath(path: Path): Unit = {
    given(value.pathMetadata(asSubPath(path))).willReturn(Future.successful(None))
  }

  def givenDirectory(path: Path): Unit = {
    givenExistingPath(DirectoryMetadata(
      path = path,
      metadata = defaultMetadataUrl,
      content = Seq.empty,
      owner = "user",
      group = "group",
      modificationTime = defaultTime,
      accessTime = defaultTime,
      permissions = defaultMask
    ))
  }

  def givenFile(path: Path): Unit = {
    givenExistingPath(FileMetadata(
      path = path,
      metadata = defaultMetadataUrl,
      content = Some(new URL(s"https://content$path")),
      owner = "user",
      group = "group",
      modificationTime = defaultTime,
      accessTime = defaultTime,
      permissions = defaultMask,
      replication = 3,
      blockSize = 2048,
      size = 0
    ))
  }

  def givenExistingPath(metadata: PathMetadata): Unit = {
    given(value.pathMetadata(metadata.path)).willReturn(Future.successful(Some(metadata)))
  }

  def givenFileCanBeCreated(path: Path): Unit = {
    givenFile(path)
    willSucceed(given(value.createFile(the(asSubPath(path)), any[PermissionsMask],
      any[Option[Short]], any[Option[Long]])))
  }
  def verifyFileCreation(
      path: Path, perms: PermissionsMask, replication: Option[Short], blockSize: Option[Long]): Unit = {
    verify(value).createFile(asSubPath(path), perms, replication, blockSize)
  }
  def verifyNoFileWasCreated(): Unit = {
    verify(value, never()).createFile(any[SubPath], any[PermissionsMask], any[Option[Short]],
      any[Option[Long]])
  }

  def givenDirectoryCanBeCreated(path: Path): Unit = {
    givenNonExistingPath(path)
    willSucceed(givenDirectoryCreation(path))
  }
  def givenDirectoryCreationWillFail(path: Path): Unit = willFail(givenDirectoryCreation(path))
  def givenDirectoryCreationWontFinish(path: Path): Unit = wontFinish(givenDirectoryCreation(path))
  private def givenDirectoryCreation(path: Path) =
    given(value.createDirectory(the(asSubPath(path)), any[PermissionsMask]))
  def verifyDirectoryCreation(path: Path, mask: PermissionsMask): Unit = {
    verify(value).createDirectory(asSubPath(path), mask)
  }

  def givenCanBeDeleted(path: Path): Unit = willSucceed(givenPathDeletion(path))
  def givenDeletionWillFail(path: Path): Unit = willFail(givenPathDeletion(path))
  private def givenPathDeletion(path: Path) = given(value.delete(the(asSubPath(path)), anyBoolean))
  def verifyDeletion(path: Path, recursive: Boolean): Unit = {
    verify(value).delete(the(asSubPath(path)), the(recursive))
  }

  def givenCanBeMoved(source: Path, target: Path): Unit = willSucceed(givenMove(source, target))
  def givenMoveWillFail(source: Path, target: Path): Unit = willFail(givenMove(source, target))
  private def givenMove(source: Path, target: Path) =
    given(value.move(asSubPath(source), asSubPath(target)))
  def verifyMove(source: Path, target: Path): Unit = {
    verify(value).move(the(asSubPath(source)), the(asSubPath(target)))
  }

  def givenOwnerCanBeChanged(path: Path): Unit = willSucceed(givenOwnerChange(path))
  def givenOwnerChangeWillFail(path: Path): Unit = willFail(givenOwnerChange(path))
  private def givenOwnerChange(path: Path) = given(value.changeOwner(the(path), any[String]))
  def givenGroupCanBeChanged(path: Path): Unit = willSucceed(givenGroupChange(path))
  def givenGroupChangeWillFail(path: Path): Unit = willFail(givenGroupChange(path))
  private def givenGroupChange(path: Path) = given(value.changeGroup(the(path), any[String]))
  def verifyOwnerChange(path: Path, owner: String): Unit = verify(value).changeOwner(path, owner)
  def verifyNotChangedOwner(path: Path): Unit = {
    verify(value, never()).changeOwner(any[Path], any[String])
  }
  def verifyGroupChange(path: Path, group: String): Unit = verify(value).changeGroup(path, group)
  def verifyNotChangedGroup(path: Path): Unit = {
    verify(value, never()).changeGroup(any[Path], any[String])
  }

  def givenMaskCanBeChanged(path: Path): Unit = willSucceed(givenMaskChange(path))
  def givenMaskChangeWillFail(path: Path): Unit = willFail(givenMaskChange(path))
  private def givenMaskChange(path: Path) =
    given(value.changePermissions(the(path), any[PermissionsMask]))
  def verifyMaskChange(path: Path, mask: PermissionsMask): Unit = {
    verify(value).changePermissions(path, mask)
  }

  def givenFileCanBeAppendedTo(path: Path): Unit = {
    givenFile(path)
    val output = new ByteArrayOutputStream()
    given(value.append(the(asSubPath(path)), anyInt)).willReturn(Future.successful(output))
  }

  private def willSucceed(call: BDDMyOngoingStubbing[Future[Unit]]): Unit =
    call.willReturn(Future.successful(()))

  private def willFail(call: BDDMyOngoingStubbing[Future[Unit]]): Unit =
    call.willReturn(Future.failed(defaultFailure))

  private def wontFinish[T](call: BDDMyOngoingStubbing[Future[T]]): Unit =
    call.willReturn(Promise[T]().future)

  private def asSubPath(path: Path): SubPath = path match {
    case RootPath => throw new IllegalArgumentException("Not a relative path")
    case subPath: SubPath => subPath
  }
}
