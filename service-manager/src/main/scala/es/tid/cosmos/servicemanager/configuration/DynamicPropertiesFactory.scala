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

package es.tid.cosmos.servicemanager.configuration


/** Source for run-time, dynamic cluster configuration. */
trait DynamicPropertiesFactory {

  /** Properties to configure services and components. */
  type ConfigProperties = Map[ConfigurationKeys.Value, String]

  /** Generate dynamic configuration properties for a given cluster.
    * This is useful for properties that differ from cluster to cluster such as host names.
    *
    * @param masterName   the cluster master node hostname
    * @param slaveNames   the cluster slave nodes hostnames
    * @return             the dynamically generated configuration properties
    * @see [[es.tid.cosmos.servicemanager.configuration.ConfigurationKeys]]
    */
  def forCluster(masterName: String, slaveNames: Seq[String]): ConfigProperties
}
