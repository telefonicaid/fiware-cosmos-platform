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
