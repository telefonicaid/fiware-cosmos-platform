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

package es.tid.cosmos.servicemanager.ambari

import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import com.typesafe.scalalogging.slf4j.Logging
import dispatch.StatusCode

import es.tid.cosmos.servicemanager.RequestException
import es.tid.cosmos.servicemanager.ambari.AmbariClusterState._
import es.tid.cosmos.servicemanager.ambari.rest.{ServiceClient, Cluster}
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDetails

trait ClusterStateResolver extends Logging {

  def resolveState(
      cluster: Cluster,
      allServices: Set[AmbariServiceDetails]): Future[AmbariClusterState] = {
    val state_> = for {
      services <- Future.traverse(cluster.serviceNames)(cluster.getService)
    } yield if (services.forall(isServiceRunning(allServices))) Running else Unknown
    state_>.recover {
      case RequestException(_, _, StatusCode(404)) => ClusterNotPresent
    }
  }

  private def isServiceRunning(allServices: Set[AmbariServiceDetails])
                              (service: ServiceClient): Boolean =
    allServices.find(_.service.name == service.name)
      .getOrElse(throw new IllegalStateException(s"Found an unknown service: ${service.name}"))
      .runningState.toString == service.state
}
