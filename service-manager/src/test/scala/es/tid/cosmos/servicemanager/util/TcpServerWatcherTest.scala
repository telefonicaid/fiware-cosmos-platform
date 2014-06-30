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

import java.net.{InetSocketAddress, ServerSocket, Socket}
import scala.concurrent.duration._

import org.scalatest.FlatSpec
import org.scalatest.concurrent.Eventually
import org.scalatest.matchers.MustMatchers
import org.scalatest.time.{Millis, Seconds, Span}

import es.tid.cosmos.common.scalatest.RandomTcpPort
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers

class TcpServerWatcherTest extends FlatSpec with MustMatchers with FutureMatchers with Eventually {

  override implicit def patienceConfig = PatienceConfig(
    timeout = Span(10, Seconds),
    interval = Span(500, Millis)
  )

  val timeout: FiniteDuration = 15.second

  trait RandomServer {
    val port = RandomTcpPort.choose()
    val watcher = TcpServerWatcher("localhost", port)

    def withServerMock[T](f: LocalServer => T): T = {
      val server = eventually {
        new LocalServer(watcher.port)
      }
      try {
        f(server)
      } finally {
        server.close()
      }
    }
  }

  "A tcp server" must "be waited for until it starts" in new RandomServer {
    withServerMock { server =>
      val f = watcher.waitForServer()
      f must not be 'completed
      Thread.sleep(1500)
      f must not be 'completed
      server.acceptClient()
      f must (runUnder(timeout) and eventuallySucceed)
    }
  }

  it must "be waited for until timeout is exceeded" in new RandomServer {
    watcher.waitForServer(1500.millis) must eventuallyFailWith("not found")
  }

  it must "handle unavailable servers" in {
    val server = TcpServerWatcher("iDonTExist", 38)
    server.waitForServer(1500.millis) must eventuallyFailWith [TcpServerNotFound]
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
}
