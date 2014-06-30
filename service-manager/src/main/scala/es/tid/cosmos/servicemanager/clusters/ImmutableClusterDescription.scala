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

package es.tid.cosmos.servicemanager.clusters

import java.net.URI

import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser}

/** An immutable description of a cluster */
case class ImmutableClusterDescription(
    override val id: ClusterId,
    override val name: ClusterName,
    override val size: Int,
    override val state: ClusterState,
    override val nameNode: Option[URI],
    override val master: Option[HostDetails],
    override val slaves: Seq[HostDetails],
    override val users: Option[Set[ClusterUser]],
    override val services: Set[String],
    override val blockedPorts: Set[Int]
) extends ClusterDescription
