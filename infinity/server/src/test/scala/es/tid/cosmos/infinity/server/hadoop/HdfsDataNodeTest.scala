/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.server.hadoop

import java.io.{OutputStream, ByteArrayInputStream, IOException, InputStream}

import org.apache.hadoop.hdfs.{DFSInputStream, DFSClient}
import org.apache.hadoop.hdfs.client.{HdfsDataOutputStream, HdfsDataInputStream}
import org.apache.hadoop.hdfs.protocol.HdfsFileStatus
import org.mockito.BDDMockito.given
import org.mockito.Matchers.{any, eq => the}
import org.mockito.Mockito.{inOrder, spy, verify}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.server.hadoop.HdfsDataNode.BoundedStreamFactory
import es.tid.cosmos.infinity.server.util.ToClose

class HdfsDataNodeTest extends FlatSpec with MustMatchers with FutureMatchers {

  "The DataNode façade" must "open an existing file for reading its content" in new OpenHappyCase {
    val stream_> = dataNode.open(path, offset = None, length = None)
    stream_> must eventually (be (ToClose(in, dfsClient)))
    verify(_dfsIn).seek(HdfsDataNode.NoOffset)
    verify(boundedStreamFactory).apply(any[HdfsDataInputStream], the(fileLength))
  }

  it must "open an existing file on given offset and for given length" in new OpenHappyCase {
    val (offset, length) = (10L, 300L)
    val stream_> = dataNode.open(path, Some(offset), Some(length))
    stream_> must eventually (be (ToClose(in, dfsClient)))
    verify(_dfsIn).seek(offset)
    verify(boundedStreamFactory).apply(any[HdfsDataInputStream], the(length))
  }

  it must "open an existing file up to its length even when given length exceeds it" in
    new OpenHappyCase {
      val length = fileLength + 1024
      val stream_> = dataNode.open(path, offset = None, Some(length))
      stream_> must eventually (be (ToClose(in, dfsClient)))
      verify(_dfsIn).seek(HdfsDataNode.NoOffset)
      verify(boundedStreamFactory).apply(any[HdfsDataInputStream], the(fileLength))
    }

  it must "fail when underlying client fails to open file" in new Fixture {
    given(clientFactory.newClient).willReturn(dfsClient)
    given(dfsClient.getFileInfo(path.toString)).willReturn(mock[HdfsFileStatus])
    given(dfsClient.open(path.toString)).willThrow(ioException)
    dataNode.open(path, offset = None, length = None) must eventuallyFailWith[IOException]
  }

  it must "fail to get content when file was not found" in new FileNotFoundCase {
    dataNode.open(path, offset = None, length = None) must
      eventuallyFailWith[DataNodeException.FileNotFound]
  }

  it must "fail to get content when path points to directory" in new PathIsDirectoryCase {
    dataNode.append(path, in) must eventuallyFailWith[DataNodeException.ContentPathIsDirectory]
  }

  it must "append the given contents to an existing file" in new AppendHappyCase {
    dataNode.append(path, in) must eventuallySucceed
    val order = inOrder(in, _dfsOut)
    order.verify(_dfsOut).write("1234".getBytes, 0, 4)
    order.verify(in).close()
    order.verify(_dfsOut).close()
  }

  it must "fail to append when client fails" in new Fixture {
    given(clientFactory.newClient).willReturn(dfsClient)
    given(dfsClient.getFileInfo(path.toString)).willReturn(mock[HdfsFileStatus])
    given(dfsClient.append(
      path.toString,
      bufferSize,
      HdfsDataNode.NoProgress,
      HdfsDataNode.NoStatistics)
    ).willThrow(ioException)
    dataNode.append(path, in) must eventuallyFailWith[IOException]
  }

  it must "fail to append when file was not found" in new FileNotFoundCase {
    dataNode.append(path, in) must eventuallyFailWith[DataNodeException.FileNotFound]
  }

  it must "fail to append when path points to directory" in new PathIsDirectoryCase {
    dataNode.append(path, in) must eventuallyFailWith[DataNodeException.ContentPathIsDirectory]
  }

  it must "overwrite an existing file with the given contents" in new OverwriteHappyCase {
    dataNode.overwrite(path, in) must eventuallySucceed
    val order = inOrder(in, _dfsOut)
    order.verify(_dfsOut).write("1234".getBytes, 0, 4)
    order.verify(in).close()
    order.verify(_dfsOut).close()
  }

  it must "fail to overwrite when client fails" in new Fixture {
    given(clientFactory.newClient).willReturn(dfsClient)
    given(dfsClient.getFileInfo(path.toString)).willReturn(mock[HdfsFileStatus])
    given(dfsClient.create(any(), any(), any(), any(), any(), any())).willThrow(ioException)
    dataNode.overwrite(path, in) must eventuallyFailWith[IOException]
  }

  it must "fail to overwrite when file was not found" in new FileNotFoundCase {
    dataNode.overwrite(path, in) must eventuallyFailWith[DataNodeException.FileNotFound]
  }

  it must "fail to overwrite when path points to directory" in new PathIsDirectoryCase {
    dataNode.overwrite(path, in) must eventuallyFailWith[DataNodeException.ContentPathIsDirectory]
  }

  trait Fixture extends MockitoSugar {
    val path = Path.absolute("/to/file")
    val fileLength = 1048576L // 1MB
    val clientFactory = mock[DfsClientFactory]("clientFactory")
    val dfsClient = mock[DFSClient]("dfsClient")
    val _dfsIn = mock[DFSInputStream]("dfsInputStream")
    val in = mock[InputStream]("resultStream")
    val bufferSize = 4
    val boundedStreamFactory = mock[BoundedStreamFactory]
    val dataNode = new HdfsDataNode(clientFactory, bufferSize, boundedStreamFactory)
    val ioException = new IOException("oops")
  }

  trait OpenHappyCase extends Fixture {
    given(clientFactory.newClient).willReturn(dfsClient)
    given(dfsClient.open(path.toString)).willReturn(_dfsIn)
    given(dfsClient.getFileInfo(path.toString)).willReturn(mock[HdfsFileStatus])
    given(_dfsIn.getFileLength).willReturn(fileLength)
    given(boundedStreamFactory.apply(any[InputStream], any[Long])).willReturn(in)
  }

  trait AppendHappyCase extends Fixture {
    override val in = spy(new ByteArrayInputStream("1234".getBytes))
    val _dfsOut = mock[HdfsDataOutputStream]("dfsOutputStream")
    given(clientFactory.newClient).willReturn(dfsClient)
    given(dfsClient.getFileInfo(path.toString)).willReturn(mock[HdfsFileStatus])
    given(dfsClient.append(
      path.toString,
      bufferSize,
      HdfsDataNode.NoProgress,
      HdfsDataNode.NoStatistics)
    ).willReturn(_dfsOut)
  }

  trait OverwriteHappyCase extends Fixture {
    override val in = spy(new ByteArrayInputStream("1234".getBytes))
    val _dfsOut = mock[OutputStream]("dfsOutputStream")
    given(clientFactory.newClient).willReturn(dfsClient)
    given(dfsClient.getFileInfo(path.toString)).willReturn(mock[HdfsFileStatus])
    given(dfsClient.create(
      the(path.toString),
      the(HdfsDataNode.DoOverwrite),
      any[Short], //replication
      any[Long], //blockSize
      the(HdfsDataNode.NoProgress),
      the(bufferSize))
    ).willReturn(_dfsOut)
  }

  trait FileNotFoundCase extends Fixture {
    given(clientFactory.newClient).willReturn(dfsClient)
    given(dfsClient.getFileInfo(path.toString)).willReturn(null)
  }

  trait PathIsDirectoryCase extends Fixture {
    val dummyDirInfo = new HdfsFileStatus(
      1,    //length
      true, //isDir
      1,    //block_replication
      1,    //blocksize
      0,    //modification_time
      0,    //access_time
      null, //permission
      "owner",
      "group",
      null, //symlink
      null, //path
      0,    //fileId
      0     //childrenNum
    )
    given(clientFactory.newClient).willReturn(dfsClient)
    given(dfsClient.getFileInfo(path.toString)).willReturn(dummyDirInfo)
  }
}
