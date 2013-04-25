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

import org.mockito.Matchers.any
import org.mockito.Mockito.{when, verify}
import org.scalatest.matchers.MustMatchers
import org.scalatest.{BeforeAndAfter, FlatSpec}
import scala.concurrent.{Future, Await}
import scala.concurrent.duration.Duration
import com.ning.http.client.Request
import net.liftweb.json.JsonAST.{JNothing, JValue}
import net.liftweb.json.JsonDSL._
import org.scalatest.mock.MockitoSugar

class AmbariServerTest extends FlatSpec with MustMatchers with BeforeAndAfter with MockitoSugar {
  var ambariServer: AmbariServer with RestResponsesComponent = _

  before {
    ambariServer = new AmbariServer("", 0, "", "") with FakeAmbariRestReplies
      with RestResponsesComponent {
      override val responses = mock[RestResponses]
      when(responses.authorize(any[Request])).thenReturn(None)
    }
  }

  it must "fail when server authentication fails" in  {
    when(ambariServer.responses.authorize(any[Request])).thenReturn(Some(
      Future.failed(ServiceException("Invalid password"))))
    evaluating {
      Await.result(ambariServer.listClusterNames, Duration.Inf)
    } must produce [ServiceException]
  }

  it must "return a list of cluster names" in {
    when(ambariServer.responses.listClusters).thenReturn(Future.successful(
      ("href" -> "http://some-bad-url") ~
      ("items" -> List("test1", "test2", "test3").map(name =>
        ("href" -> "http://another-bad-url") ~
        ("Clusters" -> (
          ("cluster_name" -> name) ~
          ("version" -> "banana")
        )))
      )))
    val clusterNames = Await.result(ambariServer.listClusterNames, Duration.Inf)
    clusterNames must have size (3)
    clusterNames(0) must be ("test1")
    clusterNames(1) must be ("test2")
    clusterNames(2) must be ("test3")
  }

  it must "return a cluster given a cluster name" in {
    when(ambariServer.responses.getCluster("test1")).thenReturn(Future.successful(
      ("href" -> "http://cluster-url") ~
      ("Clusters" -> ("cluster_name" -> "test1"))))
    Await.result(ambariServer.getCluster("test1"), Duration.Inf).name must be ("test1")
  }

  it must "propagate failures when getting a cluster" in {
    when(ambariServer.responses.getCluster("badCluster")).thenReturn(Future.failed(
      ServiceException("Cluster does not exist")))
    evaluating {
      Await.result(ambariServer.getCluster("badCluster"), Duration.Inf)
    } must produce [ServiceException]
  }

  it must "be able to create a cluster" in {
    when(ambariServer.responses.getCluster("foo")).thenReturn(Future.successful(
      ("href" -> "http://cluster-url") ~
      ("Clusters" -> ("cluster_name" -> "foo"))))
    when(ambariServer.responses.createCluster("foo")).thenReturn(Future.successful(JNothing))
    Await.result(ambariServer.createCluster("foo", "bar"), Duration.Inf).name must be ("foo")
    verify(ambariServer.responses).createCluster("foo")
  }

  it must "propagate failures when creating a cluster" in {
    when(ambariServer.responses.createCluster("test1")).thenReturn(Future.failed(
      ServiceException("Cluster already exists")))
    evaluating {
      Await.result(ambariServer.createCluster("test1", "bar"), Duration.Inf)
    } must produce [ServiceException]
  }

  it must "be able to remove a cluster" in {
    when(ambariServer.responses.removeCluster("foo")).thenReturn(Future.successful(JNothing))
    Await.result(ambariServer.removeCluster("foo"), Duration.Inf)
    verify(ambariServer.responses).removeCluster("foo")
  }

  it must "propagate failures when removing" in {
    when(ambariServer.responses.removeCluster("badcluster")).thenReturn(Future.failed(
      ServiceException("Invalid cluster")))
    evaluating {
      Await.result(ambariServer.removeCluster("badcluster"), Duration.Inf)
    } must produce [ServiceException]
  }
}
