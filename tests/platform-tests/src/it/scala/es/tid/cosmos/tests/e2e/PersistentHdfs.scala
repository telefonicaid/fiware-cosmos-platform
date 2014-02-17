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
import scala.language.postfixOps
import scala.sys.process._

import org.scalatest.Informer
import org.scalatest.matchers.MustMatchers
import org.scalatest.verb.MustVerb

class PersistentHdfs(user: User)(implicit info: Informer) extends MustVerb with MustMatchers {
  def ls: Seq[String] = (s"cosmos -c ${user.cosmosrcPath} ls /" lines_!(ProcessLogger(info(_))))
    .filterNot(_ == "No directory entries").map(line => line.substring(line.lastIndexOf(' ') + 1))

  def put(sourcePath: String, targetPath: String) {
    (s"cosmos -c ${user.cosmosrcPath} put $sourcePath $targetPath" !!).stripLineEnd must be ===
      s"$sourcePath successfully uploaded to $targetPath"
  }

  def get(remotePath: String, localPath: String): File = {
    val output = new StringBuilder
    val exitCode = s"cosmos -c ${user.cosmosrcPath} get $remotePath $localPath" !(ProcessLogger(output.append(_)))
    output.toString must include(s"downloaded to $localPath")
    exitCode must be (0)
    new File(localPath)
  }

  def rm(targetPath: String, checkOutput: Boolean = true) {
    val output = (s"cosmos -c ${user.cosmosrcPath} rm $targetPath" !!).stripLineEnd
    if (checkOutput) output must be === s"$targetPath was successfully deleted"
  }
}
