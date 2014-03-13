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

package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.util.SshKeyGenerator

/** Class for setting up a cluster user by presenting the configuration as
  * a service description.
  *
  * Information about users is sent as configuration properties prefixed by {{{user1_}}} for the
  * first user, {{{user2_}}} for the second and so on.
  */
class CosmosUserService(users: Seq[ClusterUser]) extends AmbariService {
  override val name: String = CosmosUserService.name

  override val components: Seq[ComponentDescription] = CosmosUserService.components

  override def contributions(properties: ConfigProperties): ConfigurationBundle =
    ConfigurationBundle(usersConfiguration(properties(ConfigurationKeys.MasterNode)))

  private def usersConfiguration(masterName: String) = {
    val properties = (("number_of_users" -> users.size) +: ("master" -> masterName) +: (for {
      (user, index) <- users.zipWithIndex
      sshKeys = SshKeyGenerator.newKeys(user.username, masterName)
      prefix = s"user${index + 1}_"
      pair <- Map(
        s"${prefix}username" -> user.username,
        s"${prefix}ssh_enabled" -> user.sshEnabled,
        s"${prefix}ssh_master_private_key" -> sshKeys.privateKey,
        s"${prefix}ssh_master_public_key" -> sshKeys.publicKey,
        s"${prefix}ssh_master_authorized_keys" -> s"${user.publicKey}\n${sshKeys.authorizedKey}",
        s"${prefix}ssh_slave_authorized_keys" -> sshKeys.authorizedKey,
        s"${prefix}hdfs_enabled" -> user.hdfsEnabled,
        s"${prefix}is_sudoer" -> user.isSudoer
      )
    } yield pair)).toMap

    ServiceConfiguration("cosmos-user", properties)
  }
}

object CosmosUserService extends Service with NoParametrization {
  override val name: String = "COSMOS_USER"

  override val components: Seq[ComponentDescription] = Seq(
    ComponentDescription.masterComponent("USER_MASTER_MANAGER").makeClient,
    ComponentDescription.slaveComponent("USER_SLAVE_MANAGER").makeClient
  )
}
