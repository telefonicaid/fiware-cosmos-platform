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
