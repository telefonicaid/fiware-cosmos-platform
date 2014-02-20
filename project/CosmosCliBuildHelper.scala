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

import sbt._

object CosmosCliBuildHelper {

  def make(cwd: File, log: sbt.Logger): File = {
    IO.delete(cwd / "dist")
    // TODO obtain python executable from env variable.
    exec(cwd, log, Seq("python2.7", "./bootstrap.py"))
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
