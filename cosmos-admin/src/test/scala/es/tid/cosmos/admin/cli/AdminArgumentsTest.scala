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

  val capability_name = Capability.IsOperator.toString

  class WithArguments(rawArgs: String*) {
    val args = new AdminArguments(rawArgs) {
      // Override error handle to avoid terminating the JVM if a test fail
      errorMessageHandler = message => throw new IllegalArgumentException(message)
    }
  }

  "The admin arguments" must "support the setup subcommand" in new WithArguments("setup") {
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
    "profile", "enable-capability", "--handle", "jsmith", "--capability", capability_name) {
    args.subcommands must equal (List(args.profile, args.profile.enableCapability))
    args.profile.disableCapability.handle() must be ("jsmith")
    args.profile.enableCapability.capability() must be (capability_name)
  }

  it must "support disabling a capability from a profile" in new WithArguments(
    "profile", "disable-capability", "--handle", "jsmith", "--capability", capability_name) {
      args.subcommands must equal (List(args.profile, args.profile.disableCapability))
      args.profile.disableCapability.handle() must be ("jsmith")
      args.profile.disableCapability.capability() must be (capability_name)
  }

  it must "support 'profile set-machine-quota'" in
    new WithArguments("profile", "set-machine-quota", "--handle", "jsmith", "--limit", "10") {
      args.subcommands must equal(List(args.profile, args.profile.setMachineQuota))
      args.profile.setMachineQuota.handle() must equal("jsmith")
      args.profile.setMachineQuota.limit() must equal(10)
    }

  it must "support 'profile unset-machine-quota'" in
    new WithArguments("profile", "remove-machine-quota", "--handle", "jsmith") {
      args.subcommands must equal(List(args.profile, args.profile.removeMachineQuota))
      args.profile.removeMachineQuota.handle() must equal("jsmith")
    }

  it must "support 'profile set-group'" in
    new WithArguments("profile", "set-group", "--handle", "jsmith", "--group", "supergroup") {
      args.subcommands must equal(List(args.profile, args.profile.setGroup))
      args.profile.setGroup.handle() must equal("jsmith")
      args.profile.setGroup.group() must equal("supergroup")
    }

  it must "support 'profile remove-group'" in
    new WithArguments("profile", "remove-group", "--handle", "jsmith") {
      args.subcommands must equal(List(args.profile, args.profile.removeGroup))
      args.profile.removeGroup.handle() must equal("jsmith")
    }

  it must "support 'profile list'" in new WithArguments("profile", "list") {
    args.subcommands must equal(List(args.profile, args.profile.list))
  }

  it must "support 'group create' with minimum quota" in
    new WithArguments("group", "create", "--name", "supergroup", "--min-quota", "3") {
      args.subcommands must equal(List(args.group, args.group.create))
      args.group.create.name() must equal("supergroup")
      args.group.create.minQuota() must equal(3)
    }

  it must "support 'group create' without minimum quota specified" in
    new WithArguments("group", "create", "--name", "supergroup") {
      args.subcommands must equal(List(args.group, args.group.create))
      args.group.create.name() must equal("supergroup")
      args.group.create.minQuota() must equal(0)
    }

  it must "support 'group list'" in new WithArguments("group", "list") {
    args.subcommands must equal(List(args.group, args.group.list))
  }

  it must "support 'group delete'" in new WithArguments("group", "delete", "--name", "supergroup") {
    args.subcommands must equal(List(args.group, args.group.delete))
    args.group.delete.name() must equal("supergroup")
  }

  it must "support 'group set-min-quota'" in
    new WithArguments("group", "set-min-quota", "--name", "supergroup", "--quota", "5") {
      args.subcommands must equal(List(args.group, args.group.setMinQuota))
      args.group.setMinQuota.name() must equal("supergroup")
      args.group.setMinQuota.quota() must equal(5)
    }
}
