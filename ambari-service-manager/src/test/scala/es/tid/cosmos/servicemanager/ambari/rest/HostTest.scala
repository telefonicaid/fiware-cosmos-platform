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
import net.liftweb.json.JsonDSL._
import net.liftweb.json.JsonAST.JNothing
import net.liftweb.json.{compact, render}
import org.mockito.Matchers.{eq => the, _}
import org.mockito.Mockito._
import org.scalatest.BeforeAndAfter
import org.scalatest.mock.MockitoSugar

class HostTest extends AmbariTestBase with BeforeAndAfter with MockitoSugar {
  var host: Host with MockedRestResponsesComponent = _
  var hostOf1: Host with MockedRestResponsesComponent = _
  val hostName = "testhost"

  before {
    host = new Host(
      ("Hosts" -> ("public_host_name" -> hostName) ~
      ("host_components"-> List(
        "HostRoles" -> ("component_name" -> "ExistingComponent1"),
        "HostRoles" -> ("component_name" -> "ExistingComponent2")))),
      url("http://localhost/api/v1/").build)
      with FakeAmbariRestReplies with MockedRestResponsesComponent
    hostOf1 = new Host(
        ("Hosts" -> ("public_host_name" -> hostName) ~
        ("host_components"-> List(
          "HostRoles" -> ("component_name" -> "ExistingComponent1")))),
        url("http://localhost/api/v1/").build)
      with FakeAmbariRestReplies with MockedRestResponsesComponent
  }

  it must "correctly parse the Ambari JSON host description with 1 component" in {
    hostOf1.name must be ("testhost")
    hostOf1.getComponentNames must equal(List("ExistingComponent1"))
  }

  it must "correctly parse the Ambari JSON host description with multiple components" in {
    host.name must be ("testhost")
    host.getComponentNames must equal(List("ExistingComponent1", "ExistingComponent2"))
  }

  it must "be able to add multiple components to the host" in {
    val jsonPayload = compact(render("host_components" -> List(
      "HostRoles" -> ("component_name" -> "SuperComponent1"),
      "HostRoles" -> ("component_name" -> "SuperComponent2"))))
    addMock(
      host.responses.addHostComponent(hostName, jsonPayload),
      JNothing)
    get(host.addComponents(Seq("SuperComponent1", "SuperComponent2")))
    verify(host.responses).addHostComponent(hostName, jsonPayload)
  }

  it must "be able to add a single component to the host" in {
    val jsonPayload = compact(render("host_components" -> List(
      "HostRoles" -> ("component_name" -> "SuperComponent"))))
    addMock(
      host.responses.addHostComponent(hostName, jsonPayload),
      JNothing)
    get(host.addComponents(Seq("SuperComponent")))
    verify(host.responses).addHostComponent(hostName, jsonPayload)
  }

  it must "propagate failures when adding components" in errorPropagation(
    host.responses.addHostComponent(the(hostName), any[String]),
    host.addComponents(Seq("BadComponent"))
  )

  it must "allow empty list of components without making any requests" in {
    get(host.addComponents(Seq()))
    verify(host.responses, never()).addHostComponent(any(), any())
  }
}
