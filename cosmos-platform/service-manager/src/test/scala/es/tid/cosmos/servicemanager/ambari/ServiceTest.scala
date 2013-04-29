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

import org.scalatest.BeforeAndAfter
import org.scalatest.mock.MockitoSugar
import dispatch.url
import net.liftweb.json.JsonDSL._
import net.liftweb.json.JsonAST.JNothing
import net.liftweb.json.{compact, render}
import org.mockito.Mockito._
import org.mockito.Matchers.any
import com.ning.http.client.RequestBuilder
import scala.concurrent.Future

class ServiceTest extends AmbariTestBase with BeforeAndAfter with MockitoSugar {
  val serviceName = "SuperService"
  var service: Service with MockedRestResponsesComponent = _

  before {
    service = new Service(
      ("href" -> "http://some-service-url") ~
      ("ServiceInfo" -> (
        ("cluster_name" -> "some_name") ~
        ("state" -> "INIT") ~
        ("service_name" -> serviceName)
      )) ~
      ("components" -> List()),
      url("http://localhost/api/v1/").build)
      with FakeAmbariRestReplies with MockedRestResponsesComponent with RequestHandlerFactory {
        override def createRequestHandler(url: RequestBuilder): RequestHandler = new RequestHandler {
          def ensureFinished: Future[Unit] = Future.successful()
        }
    }
  }

  it must "correctly parse the Service information JSON from Ambari" in {
    service.name must be (serviceName)
  }

  it must "be able to add components to the service" in {
    val componentName = "Component1"
    addMock(
      service.responses.addServiceComponent(componentName),
      JNothing
    )
    get(service.addComponent(componentName)) must be (componentName)
    verify(service.responses).addServiceComponent(componentName)
  }

  it must "propagate failures when adding components" in errorPropagation(
    service.responses.addServiceComponent(any[String]),
    service.addComponent("BadComponent")
  )

  def validateStateChange(state: String, operation: =>Future[Service]) {
    val jsonRequestPayload = compact(render("ServiceInfo" -> ("state" -> state)))
    addMock(
      service.responses.changeServiceState(serviceName, jsonRequestPayload),
      ("href" -> "http://www.some.url.com")
    )
    get(operation) must be (service)
    verify(service.responses).changeServiceState(serviceName, jsonRequestPayload)
  }

  def stateChangeErrorPropagation(operation: => Future[Service]) {
    errorPropagation(service.responses.changeServiceState(any[String], any[String]), operation)
  }

  it must "be able to install the service" in validateStateChange("INSTALLED", service.install())
  it must "propagate failures during install" in stateChangeErrorPropagation(service.install())
  it must "be able to start the service" in validateStateChange("STARTED", service.start())
  it must "propagate failures during start" in stateChangeErrorPropagation(service.start())
  it must "be able to stop the service" in validateStateChange("INSTALLED", service.stop())
  it must "propagate failures during stop" in stateChangeErrorPropagation(service.stop())
}
