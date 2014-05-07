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

import org.mockito.BDDMockito.given
import org.mockito.Matchers.{any, anyBoolean, eq => the}
import org.mockito.Mockito.verify
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.client.InfinityClient
import es.tid.cosmos.infinity.common.fs._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class MockInfinityClient extends MockitoSugar {

  val value = mock[InfinityClient]

  val defaultMetadataUrl = new URL("https://metadata/some/path")
  val defaultTime = new Date(449964000000L)
  val defaultMask = PermissionsMask.fromOctal("755")

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

  def givenExistingPath(metadata: PathMetadata): Unit = {
    given(value.pathMetadata(metadata.path)).willReturn(Future.successful(Some(metadata)))
  }

  def givenDirectoryCanBeCreated(path: Path): Unit = {
    givenNonExistingPath(path)
    givenDirectoryCreation(path).willReturn(Future.successful(()))
  }

  def givenDirectoryCreationWillFail(path: Path, cause: Throwable): Unit = {
    givenDirectoryCreation(path).willReturn(Future.failed(cause))
  }

  def givenDirectoryCreationWontFinish(path: Path): Unit = {
    givenDirectoryCreation(path).willReturn(never())
  }

  def givenCanBeDeleted(path: Path): Unit = {
    givenPathDeletion(path).willReturn(Future.successful(()))
  }

  def givenDeletionWillFail(path: Path, cause: Throwable): Unit = {
    givenPathDeletion(path).willReturn(Future.failed(cause))
  }

  private def givenDirectoryCreation(path: Path) =
    given(value.createDirectory(the(asSubPath(path)), any[PermissionsMask]))

  private def givenPathDeletion(path: Path) = given(value.delete(the(asSubPath(path)), anyBoolean))

  def verifyDirectoryCreation(path: Path, mask: PermissionsMask): Unit = {
    verify(value).createDirectory(asSubPath(path), mask)
  }

  def verifyDeletion(path: Path, recursive: Boolean): Unit = {
    verify(value).delete(the(asSubPath(path)), the(recursive))
  }

  private def asSubPath(path: Path): SubPath = path match {
    case RootPath => throw new IllegalArgumentException("Not a relative path")
    case subPath: SubPath => subPath
  }

  /** Creates a future that never completes */
  private def never[T](): Future[T] = Promise[T]().future
}
