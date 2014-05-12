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

package es.tid.cosmos.infinity.server.plugin

import java.util.{Map => JavaMap}
import scala.collection.JavaConverters._

import com.typesafe.config.{Config, ConfigFactory}
import org.apache.hadoop.conf.Configuration

/** Plugin configuration comes from hadoop settings (those with dfs.infinity.* prefix) taking
  * its default values from the embedded application.conf resource.
  */
private[plugin] object PluginConfig {

  private val SettingKeyPattern = """dfs\.infinity\.(.*)""".r
  private val HadoopKeyPrefix = "hadoop"

  def load(hadoopConfig: Configuration, hadoopKeys: String*): Config =
    load(hadoopConfig, hadoopKeys.toSet, ConfigFactory.load())

  def load(hadoopConfig: Configuration, hadoopKeys: Set[String], defaultConfig: Config): Config =
    filterPluginConfig(hadoopConfig, hadoopKeys).withFallback(defaultConfig)

  private def filterPluginConfig(hadoopConfig: Configuration, hadoopKeys: Set[String]): Config = {
    val settings = hadoopConfig.iterator().asScala.map(asTuple).collect {
      case (SettingKeyPattern(suffix), value) => suffix -> value
      case (key, value) if hadoopKeys.contains(key) => s"$HadoopKeyPrefix.$key" -> value
    }.toMap
    ConfigFactory.parseMap(settings.asJava)
  }

  private def asTuple(pair: JavaMap.Entry[String, String]) = (pair.getKey, pair.getValue)
}
