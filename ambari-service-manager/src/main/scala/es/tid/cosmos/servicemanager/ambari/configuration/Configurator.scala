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

package es.tid.cosmos.servicemanager.ambari.configuration

import java.util.Date
import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import es.tid.cosmos.servicemanager.ambari.rest.Cluster
import es.tid.cosmos.servicemanager.configuration._

/** Class for consolidating configuration from multiple contributors and applying it to a cluster. */
object Configurator {
  private val empty = ConfigurationBundle(None, None, List())

  /**
   * Apply the configuration offered by the given contributors to the given cluster.
   * This is done by consolidating the different pieces of configuration offered by each contributor
   * to a single contribution set and then applying it to the cluster.
   *
   * @param cluster the cluster to apply the configuration to
   * @param properties the values used to resolve dynamic configuration properties
   * @param contributors the configuration contributors offering pieces of cluster configuration
   * @return the futures of the cluster configuration application
   */
  def applyConfiguration(
    cluster: Cluster,
    properties: ConfigProperties,
    contributors: Seq[ConfigurationContributor]): Future[ConfigurationBundle] = {
    val tag = timestampedTag()
    val configurationBundle = consolidateConfiguration(contributors, properties)
    Future.traverse(configurationBundle.configurations)(cluster.applyConfiguration(_, tag)).map(
      _ => configurationBundle)
  }

  private def consolidateConfiguration(
      contributors: Seq[ConfigurationContributor],
      properties: ConfigProperties): ConfigurationBundle =
    contributors.map(_.contributions(properties)).foldLeft(empty)(_.merge(_))

  private def timestampedTag() = s"version${new Date().getTime}"
}
