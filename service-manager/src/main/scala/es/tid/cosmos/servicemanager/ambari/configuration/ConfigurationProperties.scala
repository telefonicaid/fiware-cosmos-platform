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

object ConfigurationKeys extends Enumeration {
  val MasterNode, MaxMapTasks, MaxReduceTasks = Value
}

class ConfigurationProperties(map: Map[ConfigurationKeys.Value, String]) {
  lazy val toStringMap: Map[String, String] = map.map(tuple => (tuple._1.toString, tuple._2)).toMap
}

object Implicits {
  implicit def configurationMaker(map: Map[ConfigurationKeys.Value, String]) =
    new ConfigurationProperties(map)
}
