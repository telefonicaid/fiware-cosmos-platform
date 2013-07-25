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

import es.tid.cosmos.servicemanager.{ClusterUser, ComponentDescription}
import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.util.SshKeyGenerator

/**
 * Class for setting up a cluster user by presenting the configuration as
 * a service description.
 */
class CosmosUserService(users: Seq[ClusterUser]) extends AmbariServiceDescription {
  override val name: String = CosmosUserService.name

  override val components: Seq[ComponentDescription] = CosmosUserService.components

  override def contributions(
      properties: Map[ConfigurationKeys.Value, String]): ConfigurationBundle =
    ConfigurationBundle(usersConfiguration(properties(ConfigurationKeys.MasterNode)))

  private def usersConfiguration(masterName: String) = {
    val properties = (("number_of_users" -> users.size) +: (for {
      (user, index) <- users.zipWithIndex
      sshKeys = SshKeyGenerator.newKeys(user.userName, masterName)
      prefix = s"user${index + 1}_"
      pair <- Seq(
        s"${prefix}username" -> user.userName,
        s"${prefix}master" -> masterName,
        s"${prefix}ssh_master_private_key" -> sshKeys.privateKey,
        s"${prefix}ssh_master_public_key" -> sshKeys.publicKey,
        s"${prefix}ssh_master_authorized_keys" -> user.publicKey,
        s"${prefix}ssh_slave_authorized_keys" -> sshKeys.authorizedKey)
    } yield pair)).toMap

    ServiceConfiguration("cosmos-user", properties)
  }
}

object CosmosUserService extends AmbariServiceDescription with NoConfigurationContribution {
  override val name: String = "COSMOS_USER"

  override val components: Seq[ComponentDescription] = Seq(
    ComponentDescription("USER_MASTER_MANAGER", isMaster = true, isClient = true),
    ComponentDescription("USER_SLAVE_MANAGER", isMaster = false, isClient = true)
  )
}
