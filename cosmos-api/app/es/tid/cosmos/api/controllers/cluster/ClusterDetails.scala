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

package es.tid.cosmos.api.controllers.cluster

import scala.Some

import com.wordnik.swagger.annotations.ApiProperty
import play.api.libs.json._
import play.api.mvc.RequestHeader

import es.tid.cosmos.api.profile.Cluster
import es.tid.cosmos.servicemanager.{ClusterName, ClusterUser}
import es.tid.cosmos.servicemanager.clusters.{HostDetails, ClusterDescription}
import es.tid.cosmos.servicemanager.services.{CosmosUserService, InfinityDriver}

/** A cluster from the perspective of API clients. */
case class ClusterDetails(
    href: String,
    id: String,
    @ApiProperty(dataType = "java.lang.String", notes = "Max length of 120 chars")
    name: ClusterName,
    size: Int,
    @ApiProperty(allowableValues = "provisioning,running,terminating,terminated,failed")
    state: String,
    stateDescription: String,
    master: Option[HostDetails],
    slaves: Option[Seq[HostDetails]],
    users: Option[Seq[ClusterUser]],
    shared: Boolean,
    services: Set[String],
    blockedPorts: Set[Int]
)

object ClusterDetails {

  /** Services hidden from the API user. */
  val unlistedServices: Set[String] = Set(CosmosUserService.name, InfinityDriver.name)

  /**
   * Create a ClusterDetails from a description in the context of a request.
   *
   * @param desc       Cluster description
   * @param assignment The cluster assignment from the cluster DAO
   * @param request    Context request
   * @return           A ClusterDetails instance
   */
  def apply(desc: ClusterDescription, assignment: Cluster)(implicit request: RequestHeader): ClusterDetails =
    apply(desc, assignment, ClusterResource.clusterUrl(desc.id))

  def apply(desc: ClusterDescription, assignment: Cluster, href: String): ClusterDetails =
    ClusterDetails(
      href = href,
      id = desc.id.toString,
      name = desc.name,
      size = desc.size,
      state = desc.state.name,
      stateDescription = desc.state.descLine,
      master = desc.master,
      slaves = if (desc.slaves.isEmpty) None else Some(desc.slaves),
      shared = assignment.shared,
      users = desc.users.map(_.toSeq),
      services = desc.services.filterNot(unlistedServices),
      blockedPorts = desc.blockedPorts
    )

  implicit object HostDetailsWrites extends Writes[HostDetails] {
    override def writes(info: HostDetails): JsValue = Json.obj(
        "hostname" -> info.hostname,
        "ipAddress" -> info.ipAddress
      )
  }

  implicit object ClusterUserWrites extends Writes[ClusterUser] {
    override def writes(user: ClusterUser): JsValue = Json.obj(
      "username" -> user.username,
      "sshPublicKey" -> user.publicKey,
      "isSudoer" -> user.isSudoer
    )
  }

  implicit object ClusterDetailsWrites extends Writes[ClusterDetails] {
    override def writes(d: ClusterDetails): JsValue =
      basicInfo(d) ++ machinesInfo(d) ++ usersInfo(d)

    private def basicInfo(d: ClusterDetails) = Json.obj(
      "href" -> d.href,
      "id" -> d.id,
      "name" -> d.name.underlying,
      "size" -> d.size,
      "state" -> d.state,
      "stateDescription" -> d.stateDescription,
      "shared" -> d.shared,
      "services" -> d.services.toSeq.sorted,
      "blockedPorts" -> d.blockedPorts.toSeq.sorted
    )

    private def machinesInfo(d: ClusterDetails) =
      optionalField("master" -> d.master) ++ optionalField("slaves" -> d.slaves)

    private def usersInfo(d: ClusterDetails) =
      optionalField("users" -> d.users.map(_.filter(_.isEnabled)))

    private def optionalField[T: Writes](pair: (String, Option[T])) = pair match {
      case (key, Some(value)) => Json.obj(key -> value)
      case _ => Json.obj()
    }
  }
}
