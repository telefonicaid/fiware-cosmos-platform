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

package es.tid.cosmos.infinity.client

import java.io._
import scala.concurrent.{Future, Promise}

import com.ning.http.client._
import com.ning.http.client.AsyncHandler.STATE
import org.apache.commons.logging.LogFactory

import es.tid.cosmos.infinity.common.fs.SubPath

private[client] class InputStreamHandler(path: SubPath, bufferSize: Int) extends AsyncHandler[Unit] {

  private val promise = Promise[InputStream]()
  private var bufferOpt: Option[BufferedInputStream] = None

  def future: Future[InputStream] = promise.future

  override def onThrowable(t: Throwable): Unit = {
    InputStreamHandler.Log.warn(t)
  }

  override def onStatusReceived(responseStatus: HttpResponseStatus): STATE =
    responseStatus.getStatusCode match {
      case 200 =>
        val buffer = new BufferedInputStream(bufferSize)
        promise.success(buffer)
        bufferOpt = Some(buffer)
        STATE.CONTINUE
      case 403 =>
        promise.failure(new ForbiddenException(s"Cannot read $path"))
        STATE.ABORT
      case 404 =>
        promise.failure(new NotFoundException(path))
        STATE.ABORT
      case other =>
        promise.failure(new ProtocolMismatchException(s"Unexpected server response with code $other"))
        STATE.ABORT
    }

  override def onBodyPartReceived(bodyPart: HttpResponseBodyPart): STATE =
    bufferOpt.fold(STATE.ABORT) { buffer =>
      try {
        bodyPart.writeTo(buffer.producerPipe)
        STATE.CONTINUE
      } catch {
        case _: IOException => STATE.ABORT
      }
    }

  override def onHeadersReceived(headers: HttpResponseHeaders) = STATE.CONTINUE

  override def onCompleted(): Unit = {
    bufferOpt.foreach(_.producerPipe.close())
  }
}

object InputStreamHandler {
  private val Log = LogFactory.getLog(classOf[InputStreamHandler])
}
