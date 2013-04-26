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

import com.ning.http.client.{Request, RequestBuilder}
import scala.concurrent.{Await, Future}
import net.liftweb.json.JsonAST._
import net.liftweb.json.parse
import org.mockito.Mockito._
import scala.concurrent.duration.Duration
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

trait FakeAmbariRestReplies extends JsonHttpRequest {
  this: RestResponsesComponent =>

  override def performRequest(requestBuilder: RequestBuilder): Future[JValue] = {
    val request= requestBuilder.build()
    val authorizationResponse = responses.authorize(request)
    if (!authorizationResponse.isEmpty) {
      return authorizationResponse.get
    }
    val GET = "GET"
    val POST = "POST"
    val DELETE = "DELETE"
    val PUT = "PUT"
    val AllClusters = ".+/api/v1/clusters".r
    val SpecificCluster = ".+/api/v1/clusters/([^/]+)".r
    val AllServices = ".+/api/v1/clusters/[^/]+/services".r
    val SpecificService = ".+/api/v1/clusters/[^/]+/services/([^/]+)".r
    val AllHosts = ".+/api/v1/clusters/[^/]+/hosts".r
    val SpecificHostQuery = """.+/api/v1/hosts\?Hosts%2Fhost_name=(.+)""".r
    val SpecificHost = ".+/api/v1/clusters/[^/]+/hosts/([^/]+)".r
    (request.getMethod, request.getUrl) match {
      case (GET, AllClusters()) => responses.listClusters
      case (GET, SpecificCluster(name)) => responses.getCluster(name)
      case (POST, SpecificCluster(name)) => responses.createCluster(name)
      case (DELETE, SpecificCluster(name)) => responses.removeCluster(name)
      case (PUT, SpecificCluster(name)) => responses.applyConfiguration(name, parse(request.getStringData) \\ "properties")
      case (GET, SpecificService(name)) => responses.getService(name)
      case (POST, AllServices()) => responses.addService(request.getStringData)
      case (GET, SpecificHost(name)) => responses.getHost(name)
      case (POST, AllHosts()) => responses.addHost(request.getStringData)
      case (POST, SpecificHostQuery(name)) => responses.addHostComponent(name, request.getStringData)
    }
  }
}

trait RestResponsesComponent {
  val responses: RestResponses

  trait RestResponses {
    def authorize(request: Request): Option[Future[JValue]]
    def listClusters: Future[JValue]
    def getCluster(name: String): Future[JValue]
    def createCluster(name: String): Future[JValue]
    def removeCluster(name: String): Future[JValue]
    def getService(name: String): Future[JValue]
    def addService(body: String): Future[JValue]
    def getHost(name: String): Future[JValue]
    def addHost(body: String): Future[JValue]
    def applyConfiguration(name: String, properties: JValue): Future[JValue]
    def addHostComponent(name: String, body: String): Future[JValue]
  }
}

trait AmbariTestBase extends FlatSpec with MustMatchers {
  def addMock(mockCall: () => Future[JValue], success: JValue) {
    when(mockCall()).thenReturn(Future.successful(success))
  }

  def get[T](future: Future[T]) = Await.result(future, Duration.Inf)

  def errorPropagation(mockCall: () => Future[JValue], call: () => Future[Any]) {
    when(mockCall()).thenReturn(Future.failed(ServiceException("Error")))
    evaluating {
      Await.result(call(), Duration.Inf)
    } must produce [ServiceException]
  }
}
