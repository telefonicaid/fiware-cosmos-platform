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

package es.tid.cosmos.servicemanager.ambari

import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import com.typesafe.scalalogging.slf4j.Logging
import dispatch.StatusCode

import es.tid.cosmos.servicemanager.RequestException
import es.tid.cosmos.servicemanager.ambari.AmbariClusterState._
import es.tid.cosmos.servicemanager.ambari.rest.{ServiceClient, Cluster}
import es.tid.cosmos.servicemanager.ambari.services.AmbariService

trait ClusterStateResolver extends Logging {

  def resolveState(
      cluster: Cluster,
      allServices: Set[AmbariService]): Future[AmbariClusterState] = {
    val state_> = for {
      services <- Future.traverse(cluster.serviceNames)(cluster.getService)
    } yield if (services.forall(isServiceRunning(allServices))) Running else Unknown
    state_>.recover {
      case RequestException(_, _, StatusCode(404)) => ClusterNotPresent
    }
  }

  private def isServiceRunning(allServices: Set[AmbariService])
                              (service: ServiceClient): Boolean =
    allServices.find(_.service.name == service.name)
      .getOrElse(throw new IllegalStateException(s"Found an unknown service: ${service.name}"))
      .runningState.toString == service.state
}
