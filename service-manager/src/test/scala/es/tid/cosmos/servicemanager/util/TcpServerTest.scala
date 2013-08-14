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

package es.tid.cosmos.servicemanager.util

import java.net.{Socket, ServerSocket}
import scala.concurrent.Await
import scala.concurrent.duration._
import scala.util.Random

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.common.scalatest.matchers.FutureMatchers

class TcpServerTest extends FlatSpec with MustMatchers with FutureMatchers {

  val timeout: FiniteDuration = 3 second

  class RandomServer {
    /** Random port for concurrent test execution */
    val port = 1000 + Random.nextInt(4000)
    val server = TcpServer("localhost", port)

    def withServerMock[T](f: LocalServer => T): T = {
      val serverMock = new LocalServer(server.port)
      try {
        f(serverMock)
      } finally {
        serverMock.close()
      }
    }
  }

  "A tcp server" must "be waited for until it starts" in new RandomServer {
    withServerMock { mock =>
      val f = server.waitForServer()
      f must not be ('completed)
      Thread.sleep(1500)
      f must not be ('completed)
      mock.acceptClient()
      f must runUnder(timeout)
      f must eventually (be ())
    }
  }

  it must "be waited for until timeout is exceeded" in new RandomServer {
    val f = server.waitForServer(1500 milliseconds)
    f must not be ('completed)
    val ex = evaluating {
      Await.result(f, 4 seconds)
    } must produce[RuntimeException]
    ex.getMessage must include ("not found")
  }

  it must "handle unavailable servers" in {
    val server = TcpServer("idontexist", 38)
    val f = server.waitForServer(1500 milliseconds)
    f must not be ('completed)
    val ex = evaluating {
      Await.result(f, 4 seconds)
    } must produce[TcpServerNotFound]
  }

  class LocalServer(port: Int) {
    private var socket: Option[ServerSocket] = None
    private var clientSocket: Option[Socket] = None

    def acceptClient() {
      socket = Some(new ServerSocket(port))
      socket.foreach(_.setSoTimeout(timeout.toMillis.toInt))
      clientSocket = socket.map(_.accept())
    }

    def close() {
      clientSocket.map(_.close())
      socket.map(_.close())
    }
  }
}
