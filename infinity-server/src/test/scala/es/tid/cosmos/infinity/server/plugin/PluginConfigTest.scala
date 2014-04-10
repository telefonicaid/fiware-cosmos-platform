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

import com.typesafe.config.ConfigFactory
import org.apache.hadoop.conf.Configuration
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class PluginConfigTest extends FlatSpec with MustMatchers {

  "Plugin configuration" must "have hadoop config settings with dfs.infinity. prefix" in {
    val hadoopConfig = new Configuration(false)
    hadoopConfig.set("dfs.infinity.relevant", "ok")
    hadoopConfig.set("dfs.namenode.plugins", "ko")
    val defaultConfig = ConfigFactory.parseString("")
    val pluginConfig = PluginConfig.load(hadoopConfig, defaultConfig)
    pluginConfig.hasPath("relevant") must be (true)
    pluginConfig.entrySet().size() must be (1)
  }

  it must "have default Typesafe config settings" in {
    val hadoopConfig = new Configuration(false)
    val defaultConfig = ConfigFactory.parseString("akka.sample-setting=42")
    val pluginConfig = PluginConfig.load(hadoopConfig, defaultConfig)
    pluginConfig.getInt("akka.sample-setting") must be (42)
    pluginConfig.entrySet().size() must be (1)
  }

  it must "prioritize Hadoop config over Typesafe config" in {
    val hadoopConfig = new Configuration(false)
    hadoopConfig.set("dfs.infinity.setting", "hadoop")
    val defaultConfig = ConfigFactory.parseString("setting='typesafe'")
    val pluginConfig = PluginConfig.load(hadoopConfig, defaultConfig)
    pluginConfig.getString("setting") must be ("hadoop")
    pluginConfig.entrySet().size() must be (1)
  }
}
