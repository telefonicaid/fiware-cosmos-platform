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

package es.tid.cosmos.admin.cli

import org.rogach.scallop._

import es.tid.cosmos.api.profile.Capability.Capability

class AdminArguments(args: Seq[String]) extends ScallopConf(args) {

  implicit val capabilityConverter = CapabilityConverter

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
      val capability = opt[Capability]("capability", required = true)(capabilityConverter)
    }
    val disableCapability = new Subcommand("disable-capability") {
      val handle = opt[String]("handle", required = true)
      val capability = opt[Capability]("capability", required = true)(capabilityConverter)
    }
    val setGroup = new Subcommand("set-group") {
      val handle = opt[String]("handle", required = true)
      val group = opt[String]("group", required = true)
    }
    val removeGroup = new Subcommand("remove-group") {
      val handle = opt[String]("handle", required = true)
    }
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
