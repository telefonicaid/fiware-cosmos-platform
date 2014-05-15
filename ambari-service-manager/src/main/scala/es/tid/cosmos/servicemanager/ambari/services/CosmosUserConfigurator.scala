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

package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.ClusterUser
import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.configuration.{ConfigurationBundle, ConfigurationKeys, ServiceConfiguration}
import es.tid.cosmos.servicemanager.util.SshKeyGenerator

/** Configurator for the CosmosUserService
  *
  * Information about users is sent as configuration properties prefixed by {{{user1_}}} for the
  * first user, {{{user2_}}} for the second and so on.
  */
private[services] class CosmosUserConfigurator(users: Seq[ClusterUser])
  extends ConfigurationContributor {

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
