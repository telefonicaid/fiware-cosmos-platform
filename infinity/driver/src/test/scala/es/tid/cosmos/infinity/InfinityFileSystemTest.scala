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

import java.io.{FileNotFoundException, IOException}
import java.net.{URI, URL}
import java.util.Date
import scala.concurrent.duration._

import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.fs.Path
import org.apache.hadoop.fs.permission.FsPermission
import org.apache.hadoop.security.UserGroupInformation
import org.apache.hadoop.util.Progressable
import org.apache.log4j.BasicConfigurator
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.client.{AlreadyExistsException, ForbiddenException}
import es.tid.cosmos.infinity.common.credentials.Credentials
import es.tid.cosmos.infinity.common.fs.{DirectoryEntry, DirectoryMetadata, FileMetadata, RootPath}
import es.tid.cosmos.infinity.common.hadoop.HadoopConversions._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class InfinityFileSystemTest extends FlatSpec with MustMatchers with MockitoSugar {

  BasicConfigurator.configure()

  "An infinity file system" must "have 'infinity' schema and uri" in new Fixture {
    fs.getScheme must be ("infinity")
    fs.getUri.getScheme must be ("infinity")
  }

  it must "use a default authority when missing" in new Fixture(uri = URI.create("infinity:///")) {
    fs.getUri.toString must be ("infinity://defaultAuthority/")
  }

  it must "know the home directory" in new Fixture {
    fs.getHomeDirectory must be (new Path(s"infinity://localhost:8888/$username"))
  }

  it must "have the home directory as initial working directory" in new Fixture {
    fs.getWorkingDirectory must be (fs.getHomeDirectory)
  }

  it must "update the working directory" in new Fixture {
    fs.setWorkingDirectory(new Path("/some/dir"))
    fs.getWorkingDirectory must be (new Path("infinity://localhost:8888/some/dir"))
  }

  it must "create a directory whose parent exists" in new Fixture {
    val perms = new FsPermission("755")
    val path = new Path("/parent/newdir")
    val actualPath = mappedToUserPath(path)
    client.givenDirectory(actualPath.parent.get)
    client.givenDirectoryCanBeCreated(actualPath)

    fs.mkdirs(path, perms) must be (true)

    client.verifyDirectoryCreation(mappedToUserPath(path), perms.toInfinity)
  }

  it must "create multiple directories at once" in new Fixture {
    val perms = new FsPermission("755")
    val path = new Path("/grandparent/parent/newdir")
    client.givenDirectory(mappedToUserPath(path.getParent.getParent))
    client.givenDirectoryCanBeCreated(mappedToUserPath(path.getParent))
    client.givenDirectoryCanBeCreated(mappedToUserPath(path))

    fs.mkdirs(path, perms) must be (true)

    client.verifyDirectoryCreation(mappedToUserPath(path.getParent), perms.toInfinity)
    client.verifyDirectoryCreation(mappedToUserPath(path), perms.toInfinity)
  }

  it must "report directory creation failure" in new Fixture {
    val perms = new FsPermission("755")
    val path = new Path("/parent/newdir")
    client.givenDirectory(mappedToUserPath(path.getParent))
    client.givenNonExistingPath(mappedToUserPath(path))
    client.givenDirectoryCreationWillFail(mappedToUserPath(path))

    fs.mkdirs(path, perms) must be (false)

    client.verifyDirectoryCreation(mappedToUserPath(path), perms.toInfinity)
  }

  it must "fail if directory creation times out" in new Fixture {
    val perms = new FsPermission("755")
    val path = new Path("/parent/newdir")
    client.givenDirectory(mappedToUserPath(path.getParent))
    client.givenNonExistingPath(mappedToUserPath(path))
    client.givenDirectoryCreationWontFinish(mappedToUserPath(path))

    fs.mkdirs(path, perms) must be (false)

    client.verifyDirectoryCreation(mappedToUserPath(path), perms.toInfinity)
  }

  it must "create directories with default permissions and the configured umask" in new Fixture {
    FsPermission.setUMask(conf, new FsPermission("077"))
    val path = new Path("/grandparent/parent/newdir")
    client.givenDirectory(mappedToUserPath(path.getParent.getParent))
    client.givenDirectoryCanBeCreated(mappedToUserPath(path.getParent))
    client.givenDirectoryCanBeCreated(mappedToUserPath(path))

    fs.mkdirs(path) must be (true)
    client.verifyDirectoryCreation(mappedToUserPath(path), new FsPermission("700").toInfinity)
  }

  it must "get the file status of an existing file" in new Fixture {
    client.givenExistingPath(someFileMetadata)
    fs.getFileStatus(someFile) must be (someFileMetadata.copy(path = someFile.toInfinity).toHadoop)
  }

  it must "get the file status of an existing directory" in new Fixture {
    client.givenExistingPath(someDirMetadata)
    fs.getFileStatus(someDir) must be (someDirMetadata.copy(path = someDir.toInfinity).toHadoop)
  }

  it must "throw FileNotFoundException for non existing paths" in new Fixture {
    val path = new Path("/non/existing")
    client.givenNonExistingPath(mappedToUserPath(path))
    evaluating {
      fs.getFileStatus(path)
    } must produce [FileNotFoundException]
  }

  it must "list status for files" in new Fixture {
    client.givenExistingPath(someFileMetadata)
    fs.listStatus(someFile) must be (Array(someFileMetadata.copy(path = someFile.toInfinity).toHadoop))
  }

  it must "list status of children paths for directories" in new Fixture {
    client.givenExistingPath(someDirMetadata)
    fs.listStatus(someDir) must be (someDirMetadata.content.map {
      entry => entry.copy(path = UserPathMapper.userPath(entry.path)).toHadoop
    }.toArray)
  }

  it must "throw FileNotFoundException when listing non existing paths" in new Fixture {
    val path = new Path("/cannot/list/you")
    client.givenNonExistingPath(mappedToUserPath(path))
    evaluating {
      fs.listStatus(path)
    } must produce [FileNotFoundException]
  }

  it must "not try to delete the root path" in new Fixture {
    fs.delete(new Path("/"), recursive = false) must be (false)
  }

  it must "delete subpaths" in new Fixture {
    val path = new Path("/subdir")
    val recursive = true
    client.givenCanBeDeleted(mappedToUserPath(path))
    fs.delete(path, recursive) must be (true)
    client.verifyDeletion(mappedToUserPath(path), recursive)
  }

  it must "return false on failure" in new Fixture {
    val path = new Path("/subdir")
    val recursive = false
    client.givenDeletionWillFail(mappedToUserPath(path))
    fs.delete(path, recursive) must be (false)
    client.verifyDeletion(mappedToUserPath(path), recursive)
  }

  it must "rename paths" in new Fixture {
    val source = new Path("/some/path")
    val target = new Path("/some/renamed")
    client.givenCanBeMoved(mappedToUserPath(source), mappedToUserPath(target))
    fs.rename(source, target) must be (true)
    client.verifyMove(mappedToUserPath(source), mappedToUserPath(target))
  }

  it must "return false when paths fail to be renamed" in new Fixture {
    val source = new Path("/some/path")
    val target = new Path("/some/renamed")
    client.givenMoveWillFail(mappedToUserPath(source), mappedToUserPath(target))
    fs.rename(source, target) must be (false)
    client.verifyMove(mappedToUserPath(source), mappedToUserPath(target))
  }

  it must "fail to rename the root directory" in new Fixture {
    fs.rename(new Path("/"), new Path("/other")) must be (false)
  }

  it must "do nothing when setting owner or group to null" in new Fixture {
    fs.setOwner(somePath, null, null)
    client.verifyNotChangedOwner(someMappedPath)
    client.verifyNotChangedGroup(someMappedPath)
  }

  it must "change path owner" in new Fixture {
    client.givenOwnerCanBeChanged(someMappedPath)
    fs.setOwner(somePath, "gandalf", null)
    client.verifyOwnerChange(someMappedPath, "gandalf")
    client.verifyNotChangedGroup(someMappedPath)
  }

  it must "change path group" in new Fixture {
    client.givenGroupCanBeChanged(someMappedPath)
    fs.setOwner(somePath, null, "istari")
    client.verifyNotChangedOwner(someMappedPath)
    client.verifyGroupChange(someMappedPath, "istari")
  }

  it must "throw IOException if owner change fail" in new Fixture {
    client.givenOwnerChangeWillFail(someMappedPath)
    client.givenGroupCanBeChanged(someMappedPath)
    evaluating {
      fs.setOwner(somePath, "gandalf", "istari")
    } must produce [IOException]
    client.verifyOwnerChange(someMappedPath, "gandalf")
    client.verifyNotChangedGroup(someMappedPath)
  }

  it must "throw IOException if group change fail" in new Fixture {
    client.givenOwnerCanBeChanged(someMappedPath)
    client.givenGroupChangeWillFail(someMappedPath)
    evaluating {
      fs.setOwner(somePath, "gandalf", "istari")
    } must produce [IOException]
    client.verifyOwnerChange(someMappedPath, "gandalf")
    client.verifyGroupChange(someMappedPath, "istari")
  }

  it must "change path permissions" in new Fixture {
    client.givenMaskCanBeChanged(someMappedPath)
    val mask = PermissionsMask.fromOctal("750")
    fs.setPermission(somePath, mask.toHadoop)
    client.verifyMaskChange(someMappedPath, mask)
  }

  it must "throw IOError when failing to change path permissions" in new Fixture {
    client.givenMaskChangeWillFail(someMappedPath)
    val mask = PermissionsMask.fromOctal("750")
    evaluating {
      fs.setPermission(somePath, mask.toHadoop)
    } must produce [IOException]
    client.verifyMaskChange(someMappedPath, mask)
  }

  it must "open an existing file" in new Fixture {
    client.givenExistingPath(someFileMetadata)
    fs.open(someFile) must not be null
  }

  it must "throw FileNotFoundException when opening a non existing file" in new Fixture {
    client.givenNonExistingPath(someMappedFile)
    evaluating {
      fs.open(someFile)
    } must produce [FileNotFoundException]
  }

  it must "throw IOException when opening a directory" in new Fixture {
    client.givenExistingPath(someDirMetadata)
    evaluating {
      fs.open(someDir)
    } must produce [IOException]
  }

  it must "append to an existing file" in new Fixture {
    client.givenFileCanBeAppendedTo(someMappedFile)
    fs.append(someFile, 4096, null) must not be null
  }

  it must "throw FileNotFoundException when appending to a non existing file" in new Fixture {
    client.givenNonExistingPath(someMappedFile)
    evaluating {
      fs.append(someFile, 4096, null)
    } must produce [FileNotFoundException]
  }

  it must "throw IOException when appending to a directory" in new Fixture {
    client.givenExistingPath(someDirMetadata)
    evaluating {
      fs.append(someDir, 4096, null)
    } must produce [IOException]
  }

  it must "create a new file and it's directory hierarchy if non existing" in new Fixture {
    client.givenDirectory(someMappedFile.parent.flatMap(_.parent).get)
    client.givenDirectoryCanBeCreated(someMappedFile.parent.get)
    client.givenFileCanBeCreated(someMappedFile)
    client.givenFileCanBeAppendedTo(someMappedFile)
    val mask = PermissionsMask.fromOctal("640")
    val overwrite = false
    val outputStream = fs.create(someFile, mask.toHadoop, overwrite, bufferSize, replication,
      blockSize, progressable)
    outputStream must not be null
    client.verifyFileCreation(someMappedFile, mask, Some(replication), Some(blockSize))
  }

  it must "create a new file with overwrite when the file previously existed" in new Fixture {
    client.givenCreationWillFailWith(someMappedFile, AlreadyExistsException(someMappedFile))
    client.givenDirectory(someMappedFile.parent.flatMap(_.parent).get)
    client.givenDirectoryCanBeCreated(someMappedFile.parent.get)
    client.givenFileCanBeOverwritten(someMappedFile)
    val mask = PermissionsMask.fromOctal("640")
    val overwrite = true
    val outputStream = fs.create(someFile, mask.toHadoop, overwrite, bufferSize, replication,
      blockSize, progressable)
    outputStream must not be null
    client.verifyFileCreation(someMappedFile, mask, Some(replication), Some(blockSize))
  }

  it must "create a new file with overwrite when the file did not exist" in new Fixture {
    client.givenDirectory(someMappedFile.parent.flatMap(_.parent).get)
    client.givenDirectoryCanBeCreated(someMappedFile.parent.get)
    client.givenFileCanBeCreated(someMappedFile)
    client.givenFileCanBeOverwritten(someMappedFile)
    val mask = PermissionsMask.fromOctal("640")
    val overwrite = true
    val outputStream = fs.create(someFile, mask.toHadoop, overwrite, bufferSize, replication,
      blockSize, progressable)
    outputStream must not be null
    client.verifyFileCreation(someMappedFile, mask, Some(replication), Some(blockSize))
  }

  it must "fail to create a new file with overwrite when the previous file cannot be deleted" in new Fixture {
    client.givenCreationWillFailWith(someMappedFile, ForbiddenException("Access denied", None))
    client.givenDirectory(someMappedFile.parent.flatMap(_.parent).get)
    client.givenDirectoryCanBeCreated(someMappedFile.parent.get)
    client.givenFileCanBeCreated(someMappedFile)
    client.givenFileCanBeAppendedTo(someMappedFile)
    val mask = PermissionsMask.fromOctal("640")
    val overwrite = true
    evaluating {
      fs.create(someFile, mask.toHadoop, overwrite, bufferSize, replication,
        blockSize, progressable)
    } must produce [IOException]
  }

  it must "reject creating the root directory" in new Fixture {
    val mask = PermissionsMask.fromOctal("640")
    val overwrite = false
    evaluating {
      fs.create(new Path("/"), mask.toHadoop, overwrite, bufferSize, replication, blockSize, progressable)
    } must produce [IOException]
    client.verifyNoFileWasCreated()
  }

  val username = UserGroupInformation.getCurrentUser.getUserName
  val somePath = new Path("/some/path")
  val someMappedPath = mappedToUserPath(somePath)
  val someTime = new Date(3600000L)
  val someFile = new Path("/some/file")
  val someMappedFile = mappedToUserPath(someFile)
  val someFileMetadata = FileMetadata(
    path = someMappedFile,
    metadata = new URL(s"http://metadata/user/$username/some/file"),
    content = new URL(s"http://content/user/$username/some/file"),
    owner = "user",
    group = "cosmos",
    modificationTime = new Date(3600000L),
    accessTime = Some(new Date(3600000L)),
    permissions = PermissionsMask.fromOctal("640"),
    replication = 2,
    blockSize = 1024,
    size = 0
  )
  val someDir = new Path("/dir")
  val someDirMetadata = DirectoryMetadata(
    path = mappedToUserPath(someDir),
    metadata = new URL("http://metadata/dir"),
    content = Seq(
      DirectoryEntry.file(
        path = RootPath / "user" / "dir" / "file",
        metadata = new URL("http://metadata/dir/file"),
        owner = "user",
        group = "cosmos",
        modificationTime = someTime,
        accessTime = someTime,
        permissions = PermissionsMask.fromOctal("700"),
        replication = 3,
        blockSize = 1024,
        size = 127
      ),
      DirectoryEntry.directory(
        path = RootPath / "user" / "dir" / "tmp",
        metadata = new URL("http://metadata/dir/tmp"),
        owner = "user",
        group = "cosmos",
        modificationTime = someTime,
        permissions = PermissionsMask.fromOctal("1777")
      )
    ),
    owner = "user",
    group = "cosmos",
    modificationTime = someTime,
    permissions = PermissionsMask.fromOctal("640")
  )
  val bufferSize = 2048
  val replication: Short = 3
  val blockSize = 100000L
  val progressable = new Progressable {
    override def progress(): Unit = {}
  }
  def mappedToUserPath(path: Path) = new Path("/user/", path.toString.substring(1)).toInfinity

  abstract class Fixture(uri: URI = URI.create("infinity://localhost:8888/")) {
    val client = new MockInfinityClient
    object ClientFactory extends InfinityClientFactory{
      override def build(metadataEndpoint: URL, credentials: Credentials, timeOut: FiniteDuration) =
        client.value
    }
    val conf = new Configuration(false)
    conf.setLong(InfinityConfiguration.TimeoutProperty, 10.seconds.toMillis)
    conf.set(InfinityConfiguration.DefaultAuthorityProperty, "defaultAuthority")
    conf.set(InfinityConfiguration.ClusterSecretProperty, "secret")
    val fs = new InfinityFileSystem(ClientFactory)
    fs.setConf(conf)
    fs.initialize(uri, conf)
  }
}
