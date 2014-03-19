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

import java.io.File

import com.typesafe.config.Config
import play.api._
import play.api.db.BoneCPPlugin
import play.core.SourceMapper

/** Simplified Play application that allows using Play's DB configuration and Data Access Layer.
  *
  * @param config the application configuration
  */
private[play] class DataAccessApplication(override val config: Config) extends Application
    with SimpleConfiguration with WithDefaultGlobal {

  override val mode = Mode.Prod

  override def classloader = this.getClass.getClassLoader

  override lazy val plugins = Seq(new BoneCPPlugin(this))

  override def path: File = new File(".")

  override def sources: Option[SourceMapper] = None
}
