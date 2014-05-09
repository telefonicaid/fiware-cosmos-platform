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

package es.tid.cosmos.tests.e2e

import java.io.File
import scala.language.postfixOps
import scala.sys.process._

import org.scalatest.Informer
import org.scalatest.matchers.MustMatchers
import org.scalatest.verb.MustVerb

class PersistentHdfs(user: User)(implicit info: Informer) extends MustVerb with MustMatchers {

  def ls(): Seq[String] =
    stringToProcess(s"cosmos -c ${user.cosmosrcPath} ls /")
      .lines(ProcessLogger(info(_)))
      .filterNot(_ == "No directory entries")
      .map(line => line.substring(line.lastIndexOf(' ') + 1))
      .force

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

  def rm(targetPath: String, checkOutput: Boolean = true): Unit = {
    val output = (s"cosmos -c ${user.cosmosrcPath} rm $targetPath" !!).stripLineEnd
    if (checkOutput) output must be === s"$targetPath was successfully deleted"
  }

  def chmod(permissions: String, targetPath: String): Unit = {
    s"cosmos -c ${user.cosmosrcPath} chmod $permissions $targetPath".! must be (0)
  }
}
