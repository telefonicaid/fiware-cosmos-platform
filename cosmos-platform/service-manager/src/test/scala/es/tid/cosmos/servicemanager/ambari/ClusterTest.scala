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

import dispatch.url
import net.liftweb.json.JsonAST.{JObject, JNothing, JValue}
import net.liftweb.json.JsonDSL._
import org.mockito.Matchers.any
import org.mockito.Mockito.verify
import org.scalatest.BeforeAndAfter
import org.scalatest.mock.MockitoSugar

class ClusterTest extends AmbariTestBase with BeforeAndAfter with MockitoSugar {
  var cluster: Cluster with MockedRestResponsesComponent = _

  before {
    val clusterName = "test"
    cluster = new Cluster(
      ("href" -> "some-url") ~
      ("Clusters" -> (
        ("cluster_name" -> clusterName) ~
        ("version" -> "orange"))) ~
      ("services" -> List("service1", "service2", "service3").map(name =>
        ("href" -> "service-url") ~
        ("ServiceInfo" -> (
          ("cluster_name" -> clusterName) ~
          ("service_name" -> name)
        )))) ~
      ("hosts" -> List("host1", "host2").map(name =>
        ("href" -> "host-url") ~
        ("Hosts" -> (
          ("cluster_name" -> clusterName) ~
          ("host_name" -> name)
        )))) ~
      ("configurations" -> List(("type1", "tag1"), ("type1", "tag2"), ("type2", "tag1"))
        .map(config =>
          ("href" -> "config-url") ~
          ("tag" -> config._2) ~
          ("type" -> config._1) ~
          ("Config" -> ("cluster_name" -> clusterName)))),
      url("http://localhost/api/v1/").build)
        with FakeAmbariRestReplies with MockedRestResponsesComponent
  }

  it must "correctly parse the Ambari JSON response that describes the cluster" in {
    cluster.name must be ("test")

    cluster.serviceNames must have size (3)
    cluster.serviceNames must contain ("service1")
    cluster.serviceNames must contain ("service2")
    cluster.serviceNames must contain ("service3")

    cluster.hostNames must have size (2)
    cluster.hostNames must contain ("host1")
    cluster.hostNames must contain ("host2")

    cluster.configurations must have size (3)
    cluster.configurations must contain (new Configuration("type1", "tag1"))
    cluster.configurations must contain (new Configuration("type1", "tag2"))
    cluster.configurations must contain (new Configuration("type2", "tag1"))
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
    get(cluster.applyConfiguration("type1", "tag1", properties))
    verify(cluster.responses).applyConfiguration("test", properties)
  }

  it must "propagate failures when applying configurations" in errorPropagation(
    cluster.responses.applyConfiguration(any[String], any[String]),
    cluster.applyConfiguration("type", "tag", ("some" -> "config"))
  )
}
