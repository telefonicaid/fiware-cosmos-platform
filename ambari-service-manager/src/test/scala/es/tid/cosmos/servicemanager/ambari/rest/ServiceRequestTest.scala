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

import com.ning.http.client.Request
import dispatch.url
import net.liftweb.json.JsonDSL._
import org.mockito.Mockito._

import es.tid.cosmos.servicemanager.ServiceException

class ServiceRequestTest extends AmbariTestBase {
  var dummyUrl: Request = url("http://www.some.request.url.com/api/v1/request/1").build

  it must "handle instant completions" in {
    val request = new ServiceRequest(dummyUrl)
      with FakeAmbariRestReplies with MockedRestResponsesComponent {
        addMock(
          responses.getRequest("1"),
          ("tasks" -> List(
            "Tasks" -> ("status" -> "COMPLETED")
        )))
      }
    get(request.ensureFinished)
    verify(request.responses).getRequest("1")
  }

  it must "handle waterfalled completions of multiple tasks" in {
    val request = new ServiceRequest(dummyUrl)
      with FakeAmbariRestReplies with MockedRestResponsesComponent {
      addMock(
        responses.getRequest("1"),
        "tasks" -> List(
          "Tasks" -> ("status" -> "QUEUED"),
          "Tasks" -> ("status" -> "PENDING"),
          "Tasks" -> ("status" -> "PENDING")),
        "tasks" -> List(
          "Tasks" -> ("status" -> "PENDING"),
          "Tasks" -> ("status" -> "COMPLETED"),
          "Tasks" -> ("status" -> "IN_PROGRESS")),
        "tasks" -> List(
          "Tasks" -> ("status" -> "COMPLETED"),
          "Tasks" -> ("status" -> "COMPLETED"),
          "Tasks" -> ("status" -> "COMPLETED")))
      }
    get(request.ensureFinished)
    verify(request.responses, times(3)).getRequest("1")
  }

  it must "error if a single task ends up in a failed state" in {
    val request = new ServiceRequest(dummyUrl)
      with FakeAmbariRestReplies with MockedRestResponsesComponent {
      addMock(
        responses.getRequest("1"),
        "tasks" -> List(
          "Tasks" -> ("status" -> "QUEUED"),
          "Tasks" -> ("status" -> "PENDING"),
          "Tasks" -> ("status" -> "PENDING")),
        "tasks" -> List(
          "Tasks" -> ("status" -> "PENDING"),
          "Tasks" -> ("status" -> "COMPLETED"),
          "Tasks" -> ("status" -> "IN_PROGRESS")),
        "tasks" -> List(
          "Tasks" -> ("status" -> "PENDING"),
          "Tasks" -> ("status" -> "COMPLETED"),
          "Tasks" -> ("status" -> "TIMEDOUT")))
      }
    evaluating {
      get(request.ensureFinished)
    } must produce [ServiceException]
    verify(request.responses, times(3)).getRequest("1")

    val request2 = new ServiceRequest(dummyUrl)
      with FakeAmbariRestReplies with MockedRestResponsesComponent {
      addMock(
        responses.getRequest("1"),
        "tasks" -> List(
          "Tasks" -> ("status" -> "QUEUED"),
          "Tasks" -> ("status" -> "PENDING"),
          "Tasks" -> ("status" -> "PENDING")),
        "tasks" -> List(
          "Tasks" -> ("status" -> "PENDING"),
          "Tasks" -> ("status" -> "COMPLETED"),
          "Tasks" -> ("status" -> "IN_PROGRESS")),
        "tasks" -> List(
          "Tasks" -> ("status" -> "FAILED"),
          "Tasks" -> ("status" -> "COMPLETED"),
          "Tasks" -> ("status" -> "TIMEDOUT")))
      }
    evaluating {
      get(request2.ensureFinished)
    } must produce [ServiceException]
    verify(request2.responses, times(3)).getRequest("1")
  }

}
