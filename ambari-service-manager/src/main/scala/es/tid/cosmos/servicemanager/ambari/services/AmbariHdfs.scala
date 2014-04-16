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

package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.services.Hdfs
import es.tid.cosmos.servicemanager.services.Hdfs.HdfsParameters
import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys

object AmbariHdfs extends AmbariService with FileConfiguration {

  override val service = Hdfs

  override val components: Seq[ComponentDescription] = Seq(
    ComponentDescription.masterComponent("NAMENODE"),
    ComponentDescription.slaveComponent("DATANODE"),
    ComponentDescription.masterComponent("HDFS_CLIENT").makeClient
  )

  override def extraProperties(parameters: HdfsParameters) =
    Map(ConfigurationKeys.UmaskMode -> parameters.umask)
}
