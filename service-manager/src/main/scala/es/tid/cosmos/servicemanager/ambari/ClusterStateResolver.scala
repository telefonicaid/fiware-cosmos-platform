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
import es.tid.cosmos.servicemanager.ambari.rest.{Service, Cluster}
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDescription

trait ClusterStateResolver extends Logging {
  import AmbariClusterState._

  def resolveState(
      cluster: Cluster,
      allServices: Seq[AmbariServiceDescription]): Future[AmbariClusterState] = {
    val state_> = for {
      services <- Future.traverse(cluster.serviceNames)(cluster.getService)
    } yield if (services.forall(isServiceRunning(allServices))) Running else Unknown
    state_>.recover {
      case RequestException(_, _, StatusCode(404)) => ClusterNotPresent
    }
  }

  private def isServiceRunning(
      allServices: Seq[AmbariServiceDescription])(service: Service): Boolean = {
    // FIXME: Nasty hack to unblock 0.15.0 release with existing clusters with HDP 1.x
    if (service.name == "MAPREDUCE")
      true
    else {
      val serviceDescription = allServices
        .find(_.name == service.name)
        .getOrElse(
          throw new IllegalStateException(s"Found an unknown service: ${service.name}"))
      serviceDescription.runningState.toString == service.state
    }
  }
}
