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

import java.net.{InetSocketAddress, Socket, SocketTimeoutException}
import scala.annotation.tailrec
import scala.concurrent._
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._
import scala.util.control.NonFatal

case class TcpServerWatcher(host: String, port: Int) {

  /** Wait for the TCP server to become available.
    *
    * @param timeout Timeout in seconds
    * @return        Future to be successful when the server is available
    */
  def waitForServer(timeout: FiniteDuration = TcpServerWatcher.DefaultTimeout): Future[Unit] =
    future(blocking { blockForServer(timeout) })

  private val pollingPeriod = 1.second

  @tailrec
  private def blockForServer(timeout: FiniteDuration): Unit = {
    if (isPortAvailable(pollingPeriod min timeout)) return
    if (timeout <= pollingPeriod) throw TcpServerNotFound(this)
    else blockForServer(timeout - pollingPeriod)
  }

  /** * Tries to contact the service.
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
      case NonFatal(_) =>
        Thread.sleep(timeout.toMillis)
        false
    } finally {
      s.close()
    }
  }
}

object TcpServerWatcher {
  val DefaultTimeout: FiniteDuration = 5.minutes
  val SshServicePort: Int = 22
}
