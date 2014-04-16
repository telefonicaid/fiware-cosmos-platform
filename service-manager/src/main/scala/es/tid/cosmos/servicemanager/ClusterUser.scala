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

package es.tid.cosmos.servicemanager

/**
 * Representation of a cluster user.
 *
 * @param username the user's username
 * @param publicKey the user's public key that will be used to grant the user access to the cluster
 * @param sshEnabled whether the user is allowed to SSH the cluster
 * @param hdfsEnabled whether the user is allowed to access cluster HDFS and have a home folder
 */
case class ClusterUser(
    username: String,
    publicKey: String,
    sshEnabled: Boolean = true,
    hdfsEnabled: Boolean = true,
    isSudoer: Boolean = false) {

  val isEnabled = sshEnabled && hdfsEnabled
}

object ClusterUser {

  def enabled(username: String, publicKey: String, isSudoer: Boolean = false) = ClusterUser(
    username,
    publicKey,
    sshEnabled = true,
    hdfsEnabled = true,
    isSudoer
  )

  def disabled(username: String, publicKey: String) = ClusterUser(
    username,
    publicKey,
    sshEnabled = false,
    hdfsEnabled = false,
    isSudoer = false
  )
}
