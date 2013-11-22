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

import scala.language.reflectiveCalls

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.Capability

class AdminArgumentsTest extends FlatSpec with MustMatchers {

  class WithArguments(rawArgs: String*) {
    val args = new AdminArguments(rawArgs) {
      // Override error handle to avoid terminating the JVM if a test fail
      errorMessageHandler = message => throw new IllegalArgumentException(message)
    }
  }

  it must "support the setup subcommand" in new WithArguments("setup") {
    args.subcommand must be (Some(args.setup))
  }

  it must "support the persistent-storage setup subcommand" in
    new WithArguments("persistent-storage", "setup") {
    args.subcommands must equal (
      List(args.persistentStorage, args.persistentStorage.setup))
  }

  it must "support the persistent-storage terminate subcommand" in
    new WithArguments("persistent-storage", "terminate") {
    args.subcommands must equal (
      List(args.persistentStorage, args.persistentStorage.terminate))
  }

  it must "support the cluster terminate" in
    new WithArguments("cluster", "terminate", "--clusterid", "cluster1") {
      args.subcommands must equal (List(args.cluster, args.cluster.terminate))
    }

  it must "support enabling a capability to a profile" in new WithArguments(
    "profile", "enable-capability", "--handle", "jsmith", "--capability", "is_operator") {
    args.subcommands must equal (List(args.profile, args.profile.enableCapability))
    args.profile.disableCapability.handle() must be ("jsmith")
    args.profile.enableCapability.capability() must be (Capability.IsOperator)
  }

  it must "support disabling a capability from a profile" in new WithArguments(
    "profile", "disable-capability", "--handle", "jsmith", "--capability", "is_operator") {
      args.subcommands must equal (List(args.profile, args.profile.disableCapability))
      args.profile.disableCapability.handle() must be ("jsmith")
      args.profile.disableCapability.capability() must be (Capability.IsOperator)
    }
}
