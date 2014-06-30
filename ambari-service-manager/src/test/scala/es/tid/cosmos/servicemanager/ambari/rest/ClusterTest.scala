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

import dispatch.url
import net.liftweb.json.JsonAST.{JObject, JNothing}
import net.liftweb.json.JsonDSL._
import org.mockito.Matchers.any
import org.mockito.Mockito.verify
import org.scalatest.BeforeAndAfter
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager.configuration.{HeaderOnlyConfiguration, Configuration}

class ClusterTest extends AmbariTestBase with BeforeAndAfter with MockitoSugar {
  var cluster: Cluster with MockedRestResponsesComponent = _
  var clusterOf1: Cluster with MockedRestResponsesComponent = _

  before {
    clusterOf1 = clusterWith(
          name = "testWith1",
          services = List("service1"),
          hosts = List("host1"),
          configHeaders = List(("type1", "tag1"))
    )
    cluster = clusterWith(
      name = "test",
      services = List("service1", "service2", "service3"),
      hosts = List("host1", "host2"),
      configHeaders = List(("type1", "tag1"), ("type1", "tag2"), ("type2", "tag1"))
    )
  }

  it must "correctly parse the Ambari JSON response that describes the cluster," +
    " multiple services and hosts" in {
    cluster.name must be ("test")

    cluster.serviceNames must have size (3)
    cluster.serviceNames must contain ("service1")
    cluster.serviceNames must contain ("service2")
    cluster.serviceNames must contain ("service3")

    cluster.hostNames must have size (2)
    cluster.hostNames must contain ("host1")
    cluster.hostNames must contain ("host2")

    cluster.configurations must have size (3)
    cluster.configurations must contain (HeaderOnlyConfiguration("type1", "tag1"))
    cluster.configurations must contain (HeaderOnlyConfiguration("type1", "tag2"))
    cluster.configurations must contain (HeaderOnlyConfiguration("type2", "tag1"))
  }

  it must "correctly parse the Ambari JSON response that describes the cluster," +
      " one service and one host" in {
    clusterOf1.name must be ("testWith1")

    clusterOf1.serviceNames must have size (1)
    clusterOf1.serviceNames must contain ("service1")

    clusterOf1.hostNames must have size (1)
    clusterOf1.hostNames must contain ("host1")

    clusterOf1.configurations must have size (1)
    clusterOf1.configurations must contain (HeaderOnlyConfiguration("type1", "tag1"))
  }

  it must "be able to get a service" in {
    addMock(
      cluster.responses.getService("service1"),
      ("href" -> "some-service-url") ~
      ("ServiceInfo" -> (
        ("cluster_name" -> cluster.name) ~
        ("state" -> "INIT") ~
        ("service_name" -> "service1"))))
    get(cluster.getService("service1")).name must be ("service1")
  }

  it must "propagate failures when getting a service" in errorPropagation(
    cluster.responses.getService("bad"),
    cluster.getService("bad"))

  it must "be able to add a service" in {
    addMock(
      cluster.responses.addService("""{"ServiceInfo": {"service_name": "new"}}"""),
      JNothing)
    addMock(
      cluster.responses.getService("new"),
      ("href" -> "some-service-url") ~
        ("ServiceInfo" -> (
        ("cluster_name" -> cluster.name) ~
          ("state" -> "INIT") ~
          ("service_name" -> "new"))))
    get(cluster.addService("new")).name must be ("new")
    verify(cluster.responses).addService("""{"ServiceInfo": {"service_name": "new"}}""")
  }

  it must "propagate failures when adding a service" in errorPropagation(
    cluster.responses.addService(any[String]),
    cluster.addService("bad"))

  it must "be able to add a host" in {
    addMock(
      cluster.responses.addHost("""{"Hosts":{"host_name":"host1"}}"""),
      JNothing)
    addMock(
      cluster.responses.getHost("host1"),
      ("Hosts" -> ("public_host_name" -> "host1")))
    get(cluster.addHost("host1")).name must be ("host1")
    verify(cluster.responses).addHost("""{"Hosts":{"host_name":"host1"}}""")
  }

  it must "propagate failures when adding a host" in errorPropagation(
    cluster.responses.addHost(any[String]),
    cluster.addHost("bad")
  )

  it must "be able to get hosts" in {
    addMock(
      cluster.responses.getHost("host1"),
      ("Hosts" -> ("public_host_name" -> "host1")))
    get(cluster.getHost("host1")).name must be ("host1")
  }

  it must "propagate failures when getting a host" in errorPropagation(
    cluster.responses.getHost(any[String]),
    cluster.getHost("bad")
  )

  it must "be able to apply configurations" in {
    val properties: JObject = ("test" -> "config")
    addMock(
      cluster.responses.applyConfiguration("test", properties),
      JNothing)
    get(cluster.applyConfiguration(MockConfiguration("type1", "tag1", Map("test" -> "config")), "version1"))
    verify(cluster.responses).applyConfiguration("test", properties)
  }

  it must "propagate failures when applying configurations" in errorPropagation(
    cluster.responses.applyConfiguration(any[String], any[String]),
    cluster.applyConfiguration(MockConfiguration("type1", "tag1", Map("come" -> "config")), "version1")
  )

  case class MockConfiguration(configType: String, tag: String, properties: Map[String, Any]) extends Configuration

  private def clusterWith(
    name: String,
    services: List[String], hosts: List[String], configHeaders: List[(String, String)]) =
    new Cluster(
      ("href" -> "some-url") ~
        ("Clusters" -> (
          ("cluster_name" -> name) ~
            ("version" -> "orange"))) ~
        ("services" -> services.map(name =>
          ("href" -> "service-url") ~
            ("ServiceInfo" -> (
              ("cluster_name" -> name) ~
                ("service_name" -> name)
              )))) ~
        ("hosts" -> hosts.map(name =>
          ("href" -> "host-url") ~
            ("Hosts" -> (
              ("cluster_name" -> name) ~
                ("host_name" -> name)
              )))) ~
        ("configurations" -> configHeaders.map(config =>
          ("href" -> "config-url") ~
            ("tag" -> config._2) ~
            ("type" -> config._1) ~
            ("Config" -> ("cluster_name" -> name)))),
      url("http://localhost/api/v1/").build)
      with FakeAmbariRestReplies with MockedRestResponsesComponent

}
