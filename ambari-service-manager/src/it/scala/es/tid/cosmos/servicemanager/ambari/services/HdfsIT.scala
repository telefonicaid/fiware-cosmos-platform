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

import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys._
import es.tid.cosmos.servicemanager.services.Hdfs.HdfsParameters

class HdfsIT extends ConfiguredServiceTest {

  override val dynamicProperties = Map(
    MasterNode -> "aMasterNodeName",
    HdfsReplicationFactor -> "3",
    NameNodeHttpPort -> "50070"
  )
  val parameters = HdfsParameters("123")
  override def configurator = AmbariHdfs.configurator(parameters, resourcesConfigDirectory)

  "An Hdfs service" must "have global, core and service configuration contributions" in {
    contributions.global must be('defined)
    contributions.core must be('defined)
    contributions.services must have length 1
  }

  it must "return the namenode address" in {
    contributions.services.head.properties("dfs.namenode.http-address") must
      be ("aMasterNodeName:50070")
  }

  it must "configure the umask" in {
    contributions.services.head.properties("fs.permissions.umask-mode") must be (parameters.umask)
  }

  /* Note: This is a HDFS-specific configuration needed even when Oozie is not installed */
  it must "have the oozie proxyuser group configured to be [*]" in {
    contributions.core.get.properties("hadoop.proxyuser.oozie.groups") must equal("*")
  }

  /* Note: This is a HDFS-specific configuration needed even when Oozie is not installed */
  it must "have the oozie proxyuser hosts configured to be [*]" in {
    contributions.core.get.properties("hadoop.proxyuser.oozie.hosts") must equal("*")
  }
}
