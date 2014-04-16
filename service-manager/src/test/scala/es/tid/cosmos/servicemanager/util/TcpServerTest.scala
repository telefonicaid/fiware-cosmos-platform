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

package es.tid.cosmos.servicemanager.util

import java.net.{BindException, InetSocketAddress, Socket, ServerSocket}
import scala.concurrent.Await
import scala.concurrent.duration._
import scala.language.postfixOps

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers

class TcpServerTest extends FlatSpec with MustMatchers with FutureMatchers {

  val timeout: FiniteDuration = 15 second

  class RandomServer {
    val port = availablePort()
    val server = TcpServer("localhost", port)

    def withServerMock[T](f: LocalServer => T): T = {
      val serverMock = new LocalServer(server.port)
      try {
        f(serverMock)
      } finally {
        serverMock.close()
      }
    }

    private def availablePort(): Int = {
      val socket = new ServerSocket(0)
      val port = socket.getLocalPort
      socket.close()
      port
    }
  }

  "A tcp server" must "be waited for until it starts" in new RandomServer {
    eventuallyTcpBind { withServerMock { mock =>
      val f = server.waitForServer()
      f must not be ('completed)
      Thread.sleep(1500)
      f must not be ('completed)
      mock.acceptClient()
      f must runUnder(timeout)
      f must eventually (be ())
    }}
  }

  it must "be waited for until timeout is exceeded" in new RandomServer {
    val f = server.waitForServer(1500 milliseconds)
    f must not be ('completed)
    val ex = evaluating {
      Await.result(f, Duration.Inf)
    } must produce[RuntimeException]
    ex.getMessage must include ("not found")
  }

  it must "handle unavailable servers" in {
    val server = TcpServer("idontexist", 38)
    val f = server.waitForServer(1500 milliseconds)
    f must not be ('completed)
    val ex = evaluating {
      Await.result(f, Duration.Inf)
    } must produce[TcpServerNotFound]
  }

  class LocalServer(port: Int) {
    private var socket: Option[ServerSocket] = None
    private var clientSocket: Option[Socket] = None

    def acceptClient() {
      socket = Some(new ServerSocket())
      socket.foreach(srv => {
        srv.setSoTimeout(timeout.toMillis.toInt)
        srv.setReuseAddress(true)
        srv.bind(new InetSocketAddress(port))
      })
      clientSocket = socket.map(_.accept())
    }

    def close() {
      clientSocket.map(_.close())
      socket.map(_.close())
    }
  }

  def eventuallyTcpBind(action: => Unit) {
    val maxAttemps = 5
    for (i <- 1 to maxAttemps) {
      try {
        action
        return
      } catch {
        case e: BindException =>
        case e: Throwable => throw e
      }
    }
    throw new IllegalStateException(
      s"cannot execute the test due to a BindException after $maxAttemps attempts"
    )
  }
}
