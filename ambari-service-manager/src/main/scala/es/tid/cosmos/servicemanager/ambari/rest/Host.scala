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

package es.tid.cosmos.servicemanager.ambari.rest

import scala.concurrent.Future

import com.ning.http.client.{RequestBuilder, Request}
import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json._
import net.liftweb.json.JsonAST.JString
import net.liftweb.json.JsonDSL._

import es.tid.cosmos.servicemanager.ServiceError

/**
 * Wraps Ambari's host-related REST API calls.
 *
 * @param hostInfo       the Ambari JSON response that describes the host
 * @param clusterBaseUrl the base url of the cluster
 */
private[ambari] class Host private[ambari](hostInfo: JValue, clusterBaseUrl: Request) extends RequestProcessor {
  val name = hostInfo \ "Hosts" \ "public_host_name" match {
    case JString(hostName) => hostName
    case _ =>
      throw new ServiceError("Ambari's host information response doesn't contain a " +
        "Hosts/public_host_name element")
  }

  /**
   * Given a sequence of component names, add each component to the host. The services of each of
   * the components must have been added to the cluster previously.
   */
  def addComponents(componentNames: Seq[String]): Future[Unit] = {
    def getJsonForComponent(componentName: String) =
      ("HostRoles" -> ("component_name" -> componentName))
    def ignoreResult(result: JValue) {}
    if (!componentNames.isEmpty)
      performRequest(new RequestBuilder(clusterBaseUrl) / "hosts"
          <<? Map("Hosts/host_name" -> name)
          << compact(render(("host_components" -> componentNames.map(getJsonForComponent)))))
        .map(ignoreResult)
    else Future.successful()
  }

  def getComponentNames: Seq[String] = as.FlatValues(hostInfo,  "host_components", "component_name")
}
