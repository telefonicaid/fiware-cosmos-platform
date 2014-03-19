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

package es.tid.cosmos.admin.play

import com.typesafe.config.Config
import play.api._
import play.utils.Threads

/** Simplified Play app configuration mix-in trait based on Typesafe's config. */
private[play] trait SimpleConfiguration extends WithDefaultConfiguration { self: Application =>

  def config: Config

  override protected lazy val initialConfiguration: Configuration =
    Threads.withContextClassLoader(classloader){
      Configuration(config)
    }
}
