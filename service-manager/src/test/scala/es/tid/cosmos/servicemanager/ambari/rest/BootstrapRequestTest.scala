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
import org.mockito.Mockito._

import es.tid.cosmos.servicemanager.DeploymentException

class BootstrapRequestTest extends AmbariTestBase {
  it must "handle instant completions" in {
    val request = new BootstrapRequest(
      url("http://www.some.request.url.com/api/v1/bootstrap/1").build)
      with FakeAmbariRestReplies with MockedRestResponsesComponent {
      addMock(
        responses.bootstrapRequest("1"),
        ("status" -> "SUCCESS"))
    }
    get(request.ensureFinished)
    verify(request.responses).bootstrapRequest("1")
  }

  it must "handle delayed completions" in {
    val request = new BootstrapRequest(
      url("http://www.some.request.url.com/api/v1/bootstrap/1").build)
      with FakeAmbariRestReplies with MockedRestResponsesComponent {
      addMock(
        responses.bootstrapRequest("1"),
        ("status" -> "RUNNING"),
        ("status" -> "RUNNING"),
        ("status" -> "SUCCESS"))
    }
    get(request.ensureFinished)
    verify(request.responses, times(3)).bootstrapRequest("1")
  }

  it must "handle instant failures" in {
    val request = new BootstrapRequest(
      url("http://www.some.request.url.com/api/v1/bootstrap/1").build)
      with FakeAmbariRestReplies with MockedRestResponsesComponent {
      addMock(
        responses.bootstrapRequest("1"),
        ("status" -> "ERROR"))
    }
    evaluating {
      get(request.ensureFinished)
    } must produce [DeploymentException]
    verify(request.responses).bootstrapRequest("1")
  }

  it must "handle delayed failures" in {
    val request = new BootstrapRequest(
      url("http://www.some.request.url.com/api/v1/bootstrap/1").build)
      with FakeAmbariRestReplies with MockedRestResponsesComponent {
      addMock(
        responses.bootstrapRequest("1"),
        ("status" -> "RUNNING"),
        ("status" -> "RUNNING"),
        ("status" -> "ERROR"))
    }
    evaluating {
      get(request.ensureFinished)
    } must produce [DeploymentException]
    verify(request.responses, times(3)).bootstrapRequest("1")
  }
}
