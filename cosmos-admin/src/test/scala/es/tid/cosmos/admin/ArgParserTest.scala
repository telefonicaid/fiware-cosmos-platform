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

package es.tid.cosmos.admin

import org.scalatest.matchers.MustMatchers
import org.scalatest.FlatSpec

class ArgParserTest extends FlatSpec with MustMatchers {

  it must "support the setup subcommand" in {
    val parsedArgs = new ArgParser(Seq("setup"))
    parsedArgs.subcommand must be (Some(parsedArgs.setup))
  }
}
