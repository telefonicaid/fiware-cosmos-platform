/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.client

import java.util.Date
import scala.concurrent.Future
import scala.concurrent.duration._

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.RandomTcpPort
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.infinity.client.mock.MockInfinityServer
import es.tid.cosmos.infinity.common.credentials.UserCredentials

trait HttpInfinityClientBehavior extends MustMatchers with FutureMatchers { this: FlatSpec =>

  type ClientTest = HttpInfinityClient => Future[Any]

  def canHandleCommonErrors(test: ClientTest): Unit = {
    it must "fail if connection cannot be established" in new Fixture {
      test(client) must eventuallyFailWith[ConnectionException]
    }

    it must "fail with protocol mismatch exception on 400 responses" in new Fixture {
      infinity.givenWillReturnBadRequest()
      infinity.withServer {
        test(client) must eventuallyFailWith[ProtocolMismatchException]
      }
    }

    it must "fail with forbidden exception for 403 responses" in new Fixture {
      infinity.givenWillReturnForbidden()
      infinity.withServer {
        test(client) must eventuallyFailWith[ForbiddenException]
      }
    }
  }

  def canHandleAlreadyExistsError(test: ClientTest): Unit = {
    it must "fail if file or directory already exists" in new Fixture {
      infinity.givenWillReturnAlreadyExists()
      infinity.withServer {
        test(client) must eventuallyFailWith[AlreadyExistsException]
      }
    }
  }

  def canHandleNotFoundError(test: ClientTest): Unit = {
    it must "fail when file or directory does not exist" in
      new Fixture {
        infinity.givenWillReturnNotFound()
        infinity.withServer {
          test(client) must eventuallyFailWith[NotFoundException]
        }
      }
  }

  trait Fixture {
    val aDate = new Date(1398420798000L)
    val infinity = new MockInfinityServer(metadataPort = RandomTcpPort.choose(), defaultDate = aDate)
    val credentials = new UserCredentials("key", "secret")
    val client = new HttpInfinityClient(
      infinity.metadataEndpoint, credentials, longOperationTimeout = 10.seconds)
  }
}
