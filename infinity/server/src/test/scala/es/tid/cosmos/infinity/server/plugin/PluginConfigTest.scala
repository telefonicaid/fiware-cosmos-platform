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
