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
import es.tid.cosmos.servicemanager.ambari.configuration.{ServiceConfiguration, ConfigurationBundle}
import es.tid.cosmos.servicemanager.util.SshKeyGenerator

/**
 * Class for setting up a cluster user by presenting the configuration as
 * a service description.
 */
class CosmosUserService(user: ClusterUser) extends AmbariServiceDescription {
  override val name: String = "COSMOS_USER"

  override val components: Seq[ComponentDescription] = Seq(
    ComponentDescription("USER_MASTER_MANAGER", isMaster = true),
    ComponentDescription("USER_SLAVE_MANAGER", isMaster = false)
  )

  override def contributions(masterName: String): ConfigurationBundle =
    ConfigurationBundle(userConfiguration(masterName))

  private def userConfiguration(masterName: String) = {
    val sshKeys = SshKeyGenerator.newKeys(user.userName, masterName)
    ServiceConfiguration("cosmos-user", "version1", Map(
      "user" -> user.userName,
      "master" -> masterName,
      "ssh_master_private_key" -> sshKeys.privateKey,
      "ssh_master_public_key" -> sshKeys.publicKey,
      "ssh_master_authorized_keys" -> user.publicKey,
      "ssh_slave_authorized_keys" -> sshKeys.authorizedKey
    ))
  }
}
