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

import scala.util.Properties.envOrElse

import sbt._

object CosmosCliBuildHelper {

  def make(cwd: File, log: sbt.Logger): File = {
    IO.delete(cwd / "dist")
    exec(cwd, log, Seq(envOrElse("PYTHON", "python2.7"), "./bootstrap.py"))
    exec(cwd, log, Seq("bin/buildout", "setup", ".", "bdist_egg"))
    val eggs = (cwd / "dist") * "*.egg"
    eggs.get.head
  }

  private def exec(cwd: File, log: sbt.Logger, cmd: Seq[String]): Unit = {
    (Process(cmd, Some(cwd)) ! log) match {
      case 0 => ()
      case 1 => sys.error("Unable to package python project, check output for details.")
    }
  }

}
