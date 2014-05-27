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

package es.tid.cosmos.infinity.client

import java.io._
import java.util.Date
import scala.concurrent.{Await, Future}
import scala.concurrent.duration._

import org.apache.commons.io.IOUtils
import org.apache.log4j.BasicConfigurator
import org.scalatest.FlatSpec
import org.scalatest.concurrent.Eventually
import org.scalatest.matchers.MustMatchers
import org.scalatest.time.{Millis, Seconds, Span}

import es.tid.cosmos.common.scalatest.RandomTcpPort
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.client.mock.MockInfinityServer
import es.tid.cosmos.infinity.common.credentials.UserCredentials
import es.tid.cosmos.infinity.common.fs.{Path, RootPath, SubPath}
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class HttpInfinityClientTest extends FlatSpec
    with MustMatchers with FutureMatchers with HttpInfinityClientBehavior with Eventually {

  BasicConfigurator.configure()

  "Reading metadata" must behave like {
    canHandleCommonErrors(_.pathMetadata(somePath))

    it must "succeed for an existing file" in new Fixture {
      val fileMetadata = dataFactory.fileMetadata(somePath, permissions)
      infinity.givenExistingPaths(fileMetadata)
      infinity.withServer {
        client.pathMetadata(somePath) must eventually(be(Some(fileMetadata)))
      }
    }

    it must "return nothing when file doesn't exist" in new Fixture {
      infinity.withServer {
        client.pathMetadata(somePath) must eventually(be('empty))
      }
    }

    it must "fail with protocol mismatch exception for unparseable responses" in new Fixture {
      infinity.givenWillReturnGibberish()
      infinity.withServer {
        client.pathMetadata(somePath) must eventuallyFailWith[ProtocolMismatchException]
      }
    }
  }

  "Creating a file or directory" must behave like {
    canHandleCommonErrors(_.createFile(aFile, permissions))
    canHandleAlreadyExistsError(_.createFile(aFile, permissions))
    canHandleNotFoundError(_.createFile(aFile, permissions))

    it must "create a new file" in new Fixture {
      val directoryMetadata = dataFactory.dirMetadata(somePath, permissions)
      val filePath = somePath / "newFile"
      val expectedMetadata = dataFactory.fileMetadata(filePath, permissions)
      infinity.givenExistingPaths(directoryMetadata)
      infinity.withServer {
        client.createFile(filePath, permissions) must eventuallySucceed
        client.pathMetadata(filePath) must eventually (be (Some(expectedMetadata)))
      }
    }
  }

  "Creating a directory" must behave like {
    canHandleCommonErrors(_.createDirectory(aDir, permissions))
    canHandleAlreadyExistsError(_.createDirectory(aDir, permissions))
    canHandleNotFoundError(_.createDirectory(aDir, permissions))

    it must "create a new directory" in new Fixture {
      val parentDirectory = dataFactory.dirMetadata(somePath, permissions)
      val newDirPath = somePath / "newDirectory"
      val expectedMetadata = dataFactory.dirMetadata(newDirPath, permissions)
      infinity.givenExistingPaths(parentDirectory)
      infinity.withServer {
        client.createDirectory(newDirPath, permissions) must eventuallySucceed
        client.pathMetadata(newDirPath) must eventually(be(Some(expectedMetadata)))
      }
    }
  }

  "Moving a file or directory" must behave like {
    canHandleCommonErrors(_.move(aFile, aDir))
    canHandleAlreadyExistsError(_.move(aFile, aDir))
    canHandleNotFoundError(_.move(aFile, aDir))

    it must "move an existing file to an existing directory" in new Fixture {
      val origin = dataFactory.dirMetadata(somePath, permissions)
      val destination = dataFactory.dirMetadata(Path.absolute("/some/otherpath"), permissions)
      val file = dataFactory.fileMetadata(aFile, permissions)
      infinity.givenExistingPaths(origin, destination, file)
      infinity.withServer {
        client.move(aFile, destination.path / "otherFile") must eventuallySucceed
        client.pathMetadata(destination.path / "otherFile") must eventually (be (
          Some(dataFactory.fileMetadata(destination.path / "otherFile", permissions))))
      }
    }

    it must "move an existing directory to another existing directory" in new Fixture {
      val origin = dataFactory.dirMetadata(somePath, permissions)
      val destination = dataFactory.dirMetadata(Path.absolute("/some/otherpath"), permissions)
      val directory = dataFactory.dirMetadata(aDir, permissions)
      infinity.givenExistingPaths(origin, destination, directory)
      infinity.withServer {
        client.move(aDir, destination.path / "newDir") must eventuallySucceed
        client.pathMetadata(destination.path / "newDir") must eventually (be (
          Some(dataFactory.dirMetadata(destination.path / "newDir", permissions))))
      }
    }
  }

  "Changing the owner" must behave like {
    canHandleCommonErrors(_.changeOwner(somePath, "newOwner"))
    canHandleNotFoundError(_.changeOwner(somePath, "newOwner"))

    it must "succeed for an existing file" in new Fixture {
      val parent = dataFactory.dirMetadata(somePath, permissions)
      val file = dataFactory.fileMetadata(aFile, permissions)
      infinity.givenExistingPaths(parent, file)
      infinity.withServer {
        client.changeOwner(file.path, "newOwner") must eventuallySucceed
        client.pathMetadata(file.path) must eventually (be (Some(file.copy(owner = "newOwner"))))
      }
    }
  }

  "Changing the group" must behave like {
    canHandleCommonErrors(_.changeGroup(somePath, "newGroup"))
    canHandleNotFoundError(_.changeGroup(somePath, "newGroup"))

    it must "succeed for an existing file" in new Fixture {
      val parent = dataFactory.dirMetadata(somePath, permissions)
      val file = dataFactory.fileMetadata(aFile, permissions)
      infinity.givenExistingPaths(parent, file)
      infinity.withServer {
        client.changeGroup(file.path, "newGroup") must eventuallySucceed
        client.pathMetadata(file.path) must eventually (be (Some(file.copy(group = "newGroup"))))
      }
    }
  }

  "Changing permissions" must behave like {
    val newPermissions = PermissionsMask.fromOctal("700")
    canHandleCommonErrors(_.changePermissions(somePath, newPermissions))
    canHandleNotFoundError(_.changePermissions(somePath, newPermissions))

    it must "succeed for an existing file" in new Fixture {
      val parent = dataFactory.dirMetadata(somePath, permissions)
      val file = dataFactory.fileMetadata(aFile, permissions)
      infinity.givenExistingPaths(parent, file)
      infinity.withServer {
        client.changePermissions(file.path, newPermissions) must eventuallySucceed
        client.pathMetadata(file.path) must eventually (be (
          Some(file.copy(permissions = newPermissions))))
      }
    }
  }

  "Deleting" must behave like {
    canHandleCommonErrors(_.delete(somePath, isRecursive = true))
    canHandleNotFoundError(_.delete(somePath, isRecursive = true))

    it must "succeed for an existing file" in new Fixture {
      val parent = dataFactory.dirMetadata(somePath, permissions)
      val file = dataFactory.fileMetadata(aFile, permissions)
      infinity.givenExistingPaths(parent, file)
      infinity.withServer {
        client.delete(aFile) must eventuallySucceed
        client.pathMetadata(file.path) must eventually (be ('empty))
      }
    }

    it must "succeed for an existing directory" in new Fixture {
      val parent = dataFactory.dirMetadata(somePath, permissions)
      val directory = dataFactory.dirMetadata(aDir, permissions)
      infinity.givenExistingPaths(parent, directory)
      infinity.withServer {
        client.delete(directory.path.asInstanceOf[SubPath]) must eventuallySucceed
        client.pathMetadata(directory.path) must eventually (be ('empty))
      }
    }
  }

  "Reading content" must behave like {
    canHandleCommonErrors(readFully(_, somePath))
    // when metadata is not available
    canHandleNotFoundError(readFully(_, somePath))

    it must "succeed for an existing file with content" in new Fixture {
      val parent = dataFactory.dirMetadata(somePath, permissions)
      val file = dataFactory.fileMetadata(aFile, permissions)
      infinity.givenExistingPaths(parent, file)
      infinity.givenExistingContent(file, "aContent")
      infinity.withServer {
        contentOf(readFully(client, aFile)) must be ("aContent")
      }
    }

    it must "fail to get content for directories" in new Fixture {
      val directory = dataFactory.dirMetadata(somePath, permissions)
      infinity.givenExistingPaths(directory)
      infinity.withServer {
        readFully(client, somePath) must eventuallyFailWith [IllegalArgumentException]
      }
    }
  }

  "Appending content" must behave like {
    canHandleCommonErrors(_.append(somePath, bufferSize))
    canHandleNotFoundError(_.append(somePath, bufferSize))

    it must "succeed for an existing file" in new Fixture {
      val parent = dataFactory.dirMetadata(somePath, permissions)
      val file = dataFactory.fileMetadata(aFile, permissions)
      infinity.givenExistingPaths(parent, file)
      infinity.givenExistingContent(file, "aContent")
      infinity.withServer {
        contentOf(readFully(client, aFile)) must be ("aContent")
        writeString(client.append(aFile, bufferSize), " appended")
        eventually {
          contentOf(readFully(client, aFile)) must be ("aContent appended")
        }
      }
    }
  }

  "Overwriting content" must behave like {
    canHandleCommonErrors(_.append(somePath, bufferSize))
    canHandleNotFoundError(_.append(somePath, bufferSize))

    it must "succeed for an existing file" in new Fixture {
      val parent = dataFactory.dirMetadata(somePath, permissions)
      val file = dataFactory.fileMetadata(aFile, permissions)
      infinity.givenExistingPaths(parent, file)
      infinity.givenExistingContent(file, "aContent")
      infinity.withServer {
        contentOf(readFully(client, aFile)) must be ("aContent")
        writeString(client.overwrite(aFile, bufferSize), "newContent")
        eventually {
          contentOf(readFully(client, aFile)) must be ("newContent")
        }
      }
    }
  }

  override implicit def patienceConfig = PatienceConfig(
    timeout = Span(10, Seconds),
    interval = Span(500, Millis)
  )

  val somePath = RootPath / "some" / "path"
  val aFile = somePath / "aFile"
  val aDir = somePath / "aDir"
  val aDate = new Date(1398420798000L)
  val permissions = PermissionsMask.fromOctal("644")
  val bufferSize = 1024

  def readFully(client: InfinityClient, path: SubPath) =
    client.read(path, offset = None, length = None, bufferSize = bufferSize)

  trait Fixture {
    val infinity = new MockInfinityServer(metadataPort = RandomTcpPort.choose(), defaultDate = aDate)
    val dataFactory = infinity.TestDataFactory
    val credentials = UserCredentials("key", "secret")
    val client = new HttpInfinityClient(infinity.metadataEndpoint, credentials)
    val timeOut = 10.seconds

    def contentOf(reader_> : Future[InputStream]): String = {
      val writer = new StringWriter()
      IOUtils.copy(Await.result(reader_>, timeOut), writer)
      writer.toString
    }

    def writeString(futureOutput: Future[OutputStream], content: String): Unit = {
      val output = Await.result(futureOutput, timeOut)
      val writer = new OutputStreamWriter(output)
      writer.write(content)
      writer.close()
    }
  }
}
