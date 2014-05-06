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

import java.net.{URI, URL}

import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.fs.Path
import org.apache.hadoop.fs.permission.FsPermission
import org.apache.hadoop.security.UserGroupInformation
import org.apache.log4j.BasicConfigurator
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.common.hadoop.HadoopConversions._

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
    val username = UserGroupInformation.getCurrentUser.getUserName
    fs.getHomeDirectory must be (new Path(s"infinity://localhost:8888/user/$username"))
  }

  it must "have the home directory as initial working directory" in new Fixture {
    fs.getWorkingDirectory must be (fs.getHomeDirectory)
  }

  it must "update the working directory" in new Fixture {
    fs.setWorkingDirectory(new Path("/some/dir"))
    fs.getWorkingDirectory must be (new Path("infinity://localhost:8888/some/dir"))
  }

  it must "do nothing when asked to create the root path" in new Fixture {
    fs.mkdirs(new Path("/")) must be (true)
  }

  it must "create a directory whose parent exists" in new Fixture {
    val perms = new FsPermission("755")
    val path = new Path("/parent/newdir")
    client.givenDirectory(path.getParent.toInfinity)
    client.givenDirectoryCanBeCreated(path.toInfinity)

    fs.mkdirs(path, perms) must be (true)

    client.verifyDirectoryCreation(path.toInfinity, perms.toInfinity)
  }

  it must "create multiple directories at once" in new Fixture {
    val perms = new FsPermission("755")
    val path = new Path("/grandparent/parent/newdir")
    client.givenDirectory(path.getParent.getParent.toInfinity)
    client.givenDirectoryCanBeCreated(path.getParent.toInfinity)
    client.givenDirectoryCanBeCreated(path.toInfinity)

    fs.mkdirs(path, perms) must be (true)

    client.verifyDirectoryCreation(path.getParent.toInfinity, perms.toInfinity)
    client.verifyDirectoryCreation(path.toInfinity, perms.toInfinity)
  }

  it must "report directory creation failure" in new Fixture {
    val perms = new FsPermission("755")
    val path = new Path("/parent/newdir")
    client.givenDirectory(path.getParent.toInfinity)
    client.givenNonExistingPath(path.toInfinity)
    client.givenDirectoryCreationWillFail(path.toInfinity, new Error("problem"))

    fs.mkdirs(path, perms) must be (false)

    client.verifyDirectoryCreation(path.toInfinity, perms.toInfinity)
  }

  it must "fail if directory creation times out" in new Fixture {
    val perms = new FsPermission("755")
    val path = new Path("/parent/newdir")
    client.givenDirectory(path.getParent.toInfinity)
    client.givenNonExistingPath(path.toInfinity)
    client.givenDirectoryCreationWontFinish(path.toInfinity)

    fs.mkdirs(path, perms) must be (false)

    client.verifyDirectoryCreation(path.toInfinity, perms.toInfinity)
  }


  abstract class Fixture(uri: URI = URI.create("infinity://localhost:8888/")) {
    val client = new InfinityClientMock
    object ClientFactory extends InfinityClientFactory{
      override def build(metadataEndpoint: URL) = client.value
    }
    val conf = new Configuration(false)
    conf.setInt(InfinityConfiguration.TimeoutProperty, 200)
    conf.set(InfinityConfiguration.DefaultAuthorityProperty, "defaultAuthority")
    val fs = new InfinityFileSystem(ClientFactory)
    fs.setConf(conf)
    fs.initialize(uri, conf)
  }
}
