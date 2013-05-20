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

import es.tid.cosmos.servicemanager.ComponentDescription
import es.tid.cosmos.servicemanager.ambari.configuration.{ServiceConfiguration, ConfigurationBundle}
import es.tid.cosmos.servicemanager.util.SshKeyGenerator

/**
 * Class for setting up a cluster user by presenting the configuration as
 * a service description.
 */
class CosmosUser(userName: String, passwordHash: String) extends AmbariServiceDescription {
  override val name: String = "COSMOS_USER"

  override val components: Seq[ComponentDescription] = Seq(
    ComponentDescription("USER_MASTER_MANAGER", isMaster = true),
    ComponentDescription("USER_SLAVE_MANAGER", isMaster = false)
  )

  override def contributions(masterName: String): ConfigurationBundle =
    ConfigurationBundle(userConfiguration(masterName))

  private def userConfiguration(masterName: String) = {
    val sshKeys = SshKeyGenerator.newKeys(userName, masterName)
    ServiceConfiguration("cosmos-user", "version1", Map(
      "user" -> userName,
      "password" -> passwordHash,
      "ssh_private_key" -> sshKeys.privateKey,
      "ssh_public_key" -> sshKeys.publicKey,
      "ssh_authorized_keys" -> sshKeys.authorizedKey
    ))
  }
}
