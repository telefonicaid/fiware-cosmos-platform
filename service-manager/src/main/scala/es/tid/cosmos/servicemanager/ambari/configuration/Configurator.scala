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

package es.tid.cosmos.servicemanager.ambari.configuration

import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import es.tid.cosmos.servicemanager.ambari.rest.{Host, Cluster}

/**
 * Class for consolidating configuration from multiple contributors and applying it to a cluster.
 *
 * @author adamos
 */
object Configurator {
  private val empty = ConfigurationBundle(None, None, List())

  /**
   * Apply the configuration offered by the given contributors to the given cluster.
   * This is done by consolidating the different pieces of configuration offered by each contributor
   * to a single contribution set and then applying it to the cluster.
   *
   * @param cluster the cluster to apply the configuration to
   * @param master the master of the cluster whose information might be used in the configuration
   * @param contributors the configuration contributors offering pieces of cluster configuration
   * @return the futures of the cluster configuration application
   */
  def applyConfiguration(
    cluster: Cluster, master: Host,
    contributors: Seq[ConfigurationContributor]): Future[Seq[Unit]] =
    Future.sequence(consolidateConfiguration(contributors, master).map(cluster.applyConfiguration))

  private def consolidateConfiguration(
    contributors: Seq[ConfigurationContributor], master: Host): List[Configuration] =
    contributors.map(_.contributions(master.name)).foldLeft(empty)(consolidate).configurations

  private def consolidate(
    alreadyMerged: ConfigurationBundle, toMerge: ConfigurationBundle): ConfigurationBundle = {
      val ConfigurationBundle(globalLeft, coreLeft, services) = alreadyMerged
      val ConfigurationBundle(globalRight, coreRight, servicesToAdd) = toMerge
      ConfigurationBundle(
        mergeGlobal(globalLeft, globalRight),
        mergeCore(coreLeft, coreRight),
        concatenateIfNoDuplicates(servicesToAdd, services))
    }

  private val mergeGlobal = merge(GlobalConfiguration)_
  private val mergeCore = merge(CoreConfiguration)_

  private def merge[T <: Configuration](factory: (String, Map[String, Any]) => T)
                                       (left: Option[T], right: Option[T]): Option[T] =
    (left, right) match {
      case (None, _) => right
      case (_, None) => left
      case (Some(leftValue), Some(rightValue)) => {
        val conflictingKeys = leftValue.properties.filterKeys(rightValue.properties.contains)
        if (conflictingKeys.isEmpty)
          Some(factory(leftValue.tag, leftValue.properties ++ rightValue.properties))
        else
          throw new ConfigurationConflict(s"Found keys in conflict: $conflictingKeys")
      }
    }

  private def concatenateIfNoDuplicates(
    toAdd: List[ServiceConfiguration],
    target: List[ServiceConfiguration]): List[ServiceConfiguration] = {
      val typesInConflict = toAdd.filter(candidate =>
        target.exists(candidate.configType == _.configType)).map(_.configType)
      if (typesInConflict.isEmpty)
        toAdd ++ target
      else throw new ConfigurationConflict(
        s"Already found a service configuration for type [${typesInConflict.mkString(", ")}")
  }

  class ConfigurationConflict(message: String) extends Exception(message)
}
