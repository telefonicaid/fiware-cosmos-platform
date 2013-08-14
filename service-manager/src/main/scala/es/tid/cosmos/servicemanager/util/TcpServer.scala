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

import java.net.{SocketTimeoutException, InetSocketAddress, Socket}
import scala.annotation.tailrec
import scala.concurrent._
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._
import scala.util.control.NonFatal

case class TcpServer(host: String, port: Int) {

  /**
   * Wait for the TCP server to become available.
   *
   * @param timeout Timeout in seconds
   * @return        Future to be successful when the server is available
   */
  def waitForServer(timeout: FiniteDuration = TcpServer.DefaultTimeout): Future[Unit] =
    future(blocking { blockForServer(timeout) })

  private val pollingPeriod = 1 second

  @tailrec
  private def blockForServer(timeout: FiniteDuration) {
    if (isPortAvailable(pollingPeriod min timeout)) return
    if (timeout <= pollingPeriod) throw TcpServerNotFound(this)
    else blockForServer(timeout - pollingPeriod)
  }

  /**
   * Tries to contact the service.
   *
   * @param timeout maximum waiting time
   * @return whether the service was found available
   */
  private def isPortAvailable(timeout: FiniteDuration): Boolean = {
    val s = new Socket()
    try {
      s.connect(new InetSocketAddress(host, port), timeout.toMillis.toInt)
      true
    } catch {
      case ex: SocketTimeoutException => false
      case NonFatal(_) => {
        Thread.sleep(timeout.toMillis)
        false
      }
    } finally {
      s.close()
    }
  }
}

object TcpServer {
  val DefaultTimeout: FiniteDuration = 5 minutes
  val SshService: Int = 22
}
