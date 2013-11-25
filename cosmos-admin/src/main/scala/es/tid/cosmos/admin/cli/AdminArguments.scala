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
    val unsetMachineQuota = new Subcommand("unset-machine-quota") {
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
  }
}
