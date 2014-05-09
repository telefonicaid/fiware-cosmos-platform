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

package es.tid.cosmos.common.scalatest

import scala.util.Random
import java.net.ServerSocket
import scala.annotation.tailrec
import scala.util.control.NonFatal

/** Testing helper that randomly chooses a free TCP port. */
object RandomTcpPort {

  @tailrec
  final def choose(minPort: Int = 1000, maxPort: Int = Int.MaxValue): Int = {
    require(minPort <= maxPort)
    val candidatePort = chooseBetween(minPort, maxPort)
    if (usable(candidatePort)) candidatePort
    else choose(minPort, maxPort)
  }

  private def chooseBetween(min: Int, max: Int) = min + Random.nextInt(max - min + 1)

  private def usable(port: Int): Boolean = try {
    val socket = new ServerSocket(port)
    socket.close()
    true
  } catch {
    case NonFatal(_) => false
  }
}
