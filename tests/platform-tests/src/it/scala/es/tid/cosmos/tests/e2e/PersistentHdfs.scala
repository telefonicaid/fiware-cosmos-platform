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
import scala.sys.process._

import org.scalatest.matchers.MustMatchers
import org.scalatest.verb.MustVerb

object PersistentHdfs extends MustVerb with MustMatchers {
  def ls: Seq[String] = ("cosmos ls /" lines_!)
    .filterNot(_ == "No directory entries").map(line => line.substring(line.lastIndexOf(' ') + 1))

  def put(sourcePath: String, targetPath: String) {
    (s"cosmos put $sourcePath $targetPath" !!).stripLineEnd must be ===
      s"$sourcePath successfully uploaded to $targetPath"
  }

  def get(remotePath: String, localPath: String): File = {
    (s"cosmos get $remotePath $localPath" !!) must include(s"downloaded to $localPath")
    new File(localPath)
  }

  def rm(targetPath: String, checkOutput: Boolean = true) {
    val output = (s"cosmos rm $targetPath" !!).stripLineEnd
    if (checkOutput) output must be === s"$targetPath was successfully deleted"
  }
}
