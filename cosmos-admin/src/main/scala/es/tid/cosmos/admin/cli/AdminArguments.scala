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

package es.tid.cosmos.admin.cli

import org.rogach.scallop._

class AdminArguments(args: Seq[String]) extends ScallopConf(args) {

  val setup = new Subcommand("setup")
  val persistentStorage = new Subcommand("persistent-storage") {
    val setup = new Subcommand("setup")
    val terminate = new Subcommand("terminate")
  }
  val cluster = new Subcommand("cluster") {
    val terminate = new Subcommand("terminate") {
      val clusterId = opt[String]("clusterid", required = true)
    }
  }
  val profile = new Subcommand("profile") {
    val setMachineQuota = new Subcommand("set-machine-quota") {
      val handle = opt[String]("handle", required = true)
      val limit = opt[Int]("limit", required = true)
    }
    val removeMachineQuota = new Subcommand("remove-machine-quota") {
      val handle = opt[String]("handle", required = true)
    }
    val enableCapability = new Subcommand("enable-capability") {
      val handle = opt[String]("handle", required = true)
      val capability = opt[String]("capability", required = true)
    }
    val disableCapability = new Subcommand("disable-capability") {
      val handle = opt[String]("handle", required = true)
      val capability = opt[String]("capability", required = true)
    }
    val setGroup = new Subcommand("set-group") {
      val handle = opt[String]("handle", required = true)
      val group = opt[String]("group", required = true)
    }
    val removeGroup = new Subcommand("remove-group") {
      val handle = opt[String]("handle", required = true)
    }
    val list = new Subcommand("list")
  }

  val group = new Subcommand("group") {
    val create = new Subcommand("create") {
      val name = opt[String]("name", required = true)
      val minQuota = opt[Int]("min-quota", default = Some(0))
    }
    val list = new Subcommand("list")
    val delete = new Subcommand("delete") {
      val name = opt[String]("name", required = true)
    }
    val setMinQuota = new Subcommand("set-min-quota") {
      val name =  opt[String]("name", required = true)
      val quota = opt[Int]("quota", required = true)
    }
  }
}
