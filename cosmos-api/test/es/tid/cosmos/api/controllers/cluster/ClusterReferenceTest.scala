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

package es.tid.cosmos.api.controllers.cluster

import java.util.Date
import java.sql.Timestamp

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json

import es.tid.cosmos.api.profile.Cluster
import es.tid.cosmos.servicemanager.ClusterName
import es.tid.cosmos.servicemanager.clusters.{Provisioning, ClusterId, ClusterDescription}

class ClusterReferenceTest extends FlatSpec with MustMatchers {

  val epoch = new Timestamp(0)

  object TestDescription extends ClusterDescription {
    override val id = ClusterId("clusterId")
    override val name = ClusterName("clusterName")
    override val state = Provisioning
    override def size = throw new NotImplementedError()
    override def nameNode = throw new NotImplementedError()
    override def master = throw new NotImplementedError()
    override def slaves = throw new NotImplementedError()
    override def users = throw new NotImplementedError()
    override def services = throw new NotImplementedError()
    override def blockedPorts = throw new NotImplementedError()
  }

  "A cluster reference" must ("be composed of cluster description and assignment" +
    " for the same cluster id") in {
    val assignment = Cluster(ClusterId("otherId"), 1L, epoch)
    evaluating {
      ClusterReference(TestDescription, assignment)
    } must produce [IllegalArgumentException]
  }

  "A cluster reference with an absolute URL" must "be convertible to JSON" in {
    val assignment = Cluster(TestDescription.id, 1L, epoch)
    val reference = ClusterReference(TestDescription, assignment)
    val url = "http://host/resource"
    Json.toJson(AbsoluteUriClusterReference(url, reference)) must equal (Json.obj(
      "id" -> "clusterId",
      "href" -> url,
      "name" -> "clusterName",
      "state" -> "provisioning",
      "stateDescription" -> "Cluster is acquiring and configuring resources",
      "creationDate" -> AbsoluteUriClusterReference.timestampFormat.format(epoch)
    ))
  }
}
