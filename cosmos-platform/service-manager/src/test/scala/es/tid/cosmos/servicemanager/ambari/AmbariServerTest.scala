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

import org.scalatest.matchers.MustMatchers
import org.scalatest.{BeforeAndAfter, FlatSpec}
import scala.concurrent.{Future, Await}
import scala.concurrent.duration.Duration
import com.ning.http.client.Request
import net.liftweb.json.JsonAST.{JNothing, JValue}
import net.liftweb.json.JsonDSL._

class AmbariServerTest extends FlatSpec with MustMatchers with BeforeAndAfter {
  behavior of "AmbariServer"

  trait MockAnswersComponent extends RestResponsesComponent {
    override val responses = new MockAnswers()

    class MockAnswers extends RestResponses {
      override def authorize(request: Request): Option[Future[JValue]] = {
        request.getRealm.getPassword match {
          case "goodpassword" => None
          case _ => Some(Future.failed(ServiceException("Invalid password")))
        }
      }

      var clusterNames = List("test1", "test2", "test3")

      override def listClusters = Future.successful(
        ("href" -> "http://some-bad-url") ~
        ("items" -> clusterNames.map(name =>
          ("href" -> "http://another-bad-url") ~
          ("Clusters" -> (
              ("cluster_name" -> name) ~
              ("version" -> "banana")
          )))
        ))

      override def getCluster(name: String) =
        if (clusterNames.contains(name))
          Future.successful(
            ("href" -> "http://cluster-url") ~
            ("Clusters" -> ("cluster_name" -> name)))
        else
          Future.failed(ServiceException("Cluster does not exist"))

      override def createCluster(name: String) =
        if (clusterNames.contains(name))
          Future.failed(ServiceException("Cluster already exists"))
        else {
          clusterNames = name :: clusterNames
          Future.successful(JNothing)
        }

      override def removeCluster(name: String) =
      if (clusterNames.contains(name)) {
        clusterNames = clusterNames.filterNot(name.equals)
        Future.successful(JNothing)
      } else
        Future.failed(ServiceException("Cluster does not exist"))
    }
  }

  var ambariServer: AmbariServer = _

  before {
    ambariServer = new AmbariServer("localhost", 8080, "user", "goodpassword")
    with FakeAmbariRestReplies with MockAnswersComponent
  }

  it must "fail when a bad password is provided" in  {
    val ambariServer = new AmbariServer("localhost", 8080, "user", "badpassword")
      with FakeAmbariRestReplies with MockAnswersComponent
    val future = ambariServer.listClusterNames
    evaluating { Await.result(future, Duration.Inf) } must produce [ServiceException]
  }

  it must "return a list of cluster names" in {
    val clusterNames = Await.result(ambariServer.listClusterNames, Duration.Inf)
    clusterNames must have size (3)
    clusterNames(0) must be ("test1")
    clusterNames(1) must be ("test2")
    clusterNames(2) must be ("test3")
  }

  it must "return a cluster given a cluster name" in {
    Await.result(ambariServer.getCluster("test1"), Duration.Inf).name must be ("test1")
    Await.result(ambariServer.getCluster("test2"), Duration.Inf).name must be ("test2")
    Await.result(ambariServer.getCluster("test3"), Duration.Inf).name must be ("test3")
  }

  it must "fail when getting a non-existent cluster" in {
    evaluating {
      Await.result(ambariServer.getCluster("badCluster"), Duration.Inf)
    } must produce [ServiceException]
  }

  it must "be able to create a cluster" in {
    Await.result(ambariServer.createCluster("foo", "bar"), Duration.Inf).name must be ("foo")
    Await.result(ambariServer.listClusterNames, Duration.Inf) must contain ("foo")
  }

  it must "fail when creating a cluster that already exists" in {
    evaluating {
      Await.result(ambariServer.createCluster("test1", "bar"), Duration.Inf)
    } must produce [ServiceException]
  }

  it must "be able to remove a cluster" in {
    val clusterNames = Await.result(ambariServer.listClusterNames, Duration.Inf)
    clusterNames must contain ("test1")
    Await.result(ambariServer.removeCluster("test1"), Duration.Inf)
    val newClusterNames = Await.result(ambariServer.listClusterNames, Duration.Inf)
    newClusterNames must not contain ("test1")
    newClusterNames must have size (clusterNames.size - 1)
  }

  it must "fail when remove a cluster that doesn't exists" in {
    evaluating {
      Await.result(ambariServer.removeCluster("badcluster"), Duration.Inf)
    } must produce [ServiceException]
  }
}
