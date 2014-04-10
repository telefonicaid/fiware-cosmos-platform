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

  def load(hadoopConfig: Configuration): Config = load(hadoopConfig, ConfigFactory.load())

  def load(hadoopConfig: Configuration, defaultConfig: Config): Config =
    filterPluginConfig(hadoopConfig).withFallback(defaultConfig)

  private def filterPluginConfig(hadoopConfig: Configuration): Config = {
    val settings = hadoopConfig.iterator().asScala.map(asTuple).collect {
      case (SettingKeyPattern(suffix), value) => suffix -> value
    }.toMap
    ConfigFactory.parseMap(settings.asJava)
  }

  private def asTuple(pair: JavaMap.Entry[String, String]) = (pair.getKey, pair.getValue)
}
