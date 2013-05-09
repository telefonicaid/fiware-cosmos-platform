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
  val hostName = "testhost"

  before {
    host = new Host(
      ("Hosts" -> ("public_host_name" -> hostName)),
      url("http://localhost/api/v1/").build)
      with FakeAmbariRestReplies with MockedRestResponsesComponent
  }

  it must "correctly parse the Ambari JSON host description" in  {
    host.name must be ("testhost")
  }

  it must "be able to add multiple components to the host" in {
    val jsonPayload = compact(render("host_components" -> List(
      "HostRoles" -> ("component_name" -> "SuperComponent1"),
      "HostRoles" -> ("component_name" -> "SuperComponent2"))))
    addMock(
      host.responses.addHostComponent(hostName, jsonPayload),
      JNothing)
    get(host.addComponents("SuperComponent1", "SuperComponent2"))
    verify(host.responses).addHostComponent(hostName, jsonPayload)
  }

  it must "be able to add a single component to the host" in {
    val jsonPayload = compact(render("host_components" -> List(
      "HostRoles" -> ("component_name" -> "SuperComponent"))))
    addMock(
      host.responses.addHostComponent(hostName, jsonPayload),
      JNothing)
    get(host.addComponents("SuperComponent"))
    verify(host.responses).addHostComponent(hostName, jsonPayload)
  }

  it must "propagate failures when adding components" in errorPropagation(
    host.responses.addHostComponent(the(hostName), any[String]),
    host.addComponents("BadComponent")
  )

  it must "allow empty list of components without making any requests" in {
    get(host.addComponents())
    verify(host.responses, never()).addHostComponent(any(), any())
  }
}
