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
import scala.concurrent.Future
import net.liftweb.json.JsonAST.JValue
import es.tid.cosmos.servicemanager.InternalError

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
    val AllClusters = ".+/api/v1/clusters".r
    val SpecificCluster = ".+/api/v1/clusters/([^/]+)".r
    (request.getMethod, request.getUrl) match {
      case (GET, AllClusters()) => responses.listClusters
      case (GET, SpecificCluster(name)) => responses.getCluster(name)
      case (POST, SpecificCluster(name)) => responses.createCluster(name)
      case (DELETE, SpecificCluster(name)) => responses.removeCluster(name)
    }
  }
}

trait RestResponsesComponent {
  val responses: RestResponses

  trait RestResponses {
    def authorize(request: Request): Option[Future[JValue]] = None
    def listClusters: Future[JValue] = notImplemented
    def getCluster(name: String): Future[JValue] = notImplemented
    def createCluster(name: String): Future[JValue] = notImplemented
    def removeCluster(name: String): Future[JValue] = notImplemented

    private def notImplemented: Future[JValue] = Future.failed[JValue](InternalError("Test error, not implemented"))
  }
}
