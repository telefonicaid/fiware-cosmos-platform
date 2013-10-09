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

package es.tid.cosmos.tests.e2e

import java.io.File
import java.io.File.createTempFile

import org.scalatest.{BeforeAndAfter, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.common.scalatest.tags.EndToEndTest

class PersistentHdfsAccessIT extends FlatSpec with MustMatchers with BeforeAndAfter {
  val source = {
    val file = createTempFile("test", "txt")
    file.deleteOnExit()
    file.getAbsolutePath
  }
  val target = "testInHdfs.txt"
  var localCopy: File = _

  before {
    ensureCleanHdfsDirectory()
    localCopy = createTempFile("localCopy", "txt")
    localCopy.delete() must be (true)
  }

  after  {
    ensureCleanHdfsDirectory()
    localCopy.delete() must be (true)
  }

  "The peristent HDFS cluster" must "be accessible by a cosmos user" taggedAs EndToEndTest in {
    PersistentHdfs.ls must be ('empty)
    PersistentHdfs.put(source, target)
    PersistentHdfs.ls must be (Stream(target))
    PersistentHdfs.get(target, localCopy.getAbsolutePath).exists must be (true)
    PersistentHdfs.rm(target)
    PersistentHdfs.ls must be ('empty)
  }

  def ensureCleanHdfsDirectory() { PersistentHdfs.rm(target, checkOutput = false) }
}
