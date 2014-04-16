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

import com.ning.http.client.RequestBuilder
import dispatch.url
import net.liftweb.json.JsonDSL._
import net.liftweb.json.JsonAST.JNothing
import net.liftweb.json.{compact, render}
import org.mockito.Mockito._
import org.mockito.Matchers.any
import org.scalatest.BeforeAndAfter
import org.scalatest.mock.MockitoSugar

class ServiceClientTest extends AmbariTestBase with BeforeAndAfter with MockitoSugar {
  val serviceName = "SuperService"
  var service: ServiceClient with MockedRestResponsesComponent = _

  before {
    service = new ServiceClient(infoWithState("INIT"), url("http://localhost/api/v1/").build)
      with FakeAmbariRestReplies with MockedRestResponsesComponent with RequestHandlerFactory {
        override def createRequestHandler(url: RequestBuilder): RequestHandler =
          new RequestHandler {
            def ensureFinished: Future[Unit] = Future.successful()
          }
    }
  }

  it must "correctly parse the Service information JSON from Ambari" in {
    service.name must be (serviceName)
    service.state must be ("INIT")
  }

  it must "be able to add components to the service" in {
    val componentName = "Component1"
    addMock(
      service.responses.addServiceComponent(componentName),
      JNothing)
    get(service.addComponent(componentName)) must be (componentName)
    verify(service.responses).addServiceComponent(componentName)
  }

  it must "propagate failures when adding components" in errorPropagation(
    service.responses.addServiceComponent(any[String]),
    service.addComponent("BadComponent")
  )

  def validateStateChange(state: String, operation: =>Future[ServiceClient]) {
    val jsonRequestPayload = compact(render("ServiceInfo" -> ("state" -> state)))
    addMock(
      service.responses.changeServiceState(serviceName, jsonRequestPayload),
      ("href" -> "http://www.some.url.com"))
    addMock(service.responses.getService(service.name), infoWithState(state))
    get(operation) must be (service)
    verify(service.responses).changeServiceState(serviceName, jsonRequestPayload)
  }

  def stateChangeErrorPropagation(operation: => Future[ServiceClient]) {
    errorPropagation(service.responses.changeServiceState(any[String], any[String]), operation)
  }

  def infoWithState(state: String) = ("href" -> "http://some-service-url") ~
    ("ServiceInfo" -> (
      ("cluster_name" -> "some_name") ~
      ("state" -> s"$state") ~
      ("service_name" -> serviceName))) ~
    ("components" -> List())

  it must "be able to install the service" in validateStateChange("INSTALLED", service.install())
  it must "propagate failures during install" in stateChangeErrorPropagation(service.install())
  it must "be able to start the service" in validateStateChange("STARTED", service.start())
  it must "propagate failures during start" in stateChangeErrorPropagation(service.start())
  it must "be able to stop the service" in validateStateChange("INSTALLED", service.stop())
  it must "propagate failures during stop" in stateChangeErrorPropagation(service.stop())
}
