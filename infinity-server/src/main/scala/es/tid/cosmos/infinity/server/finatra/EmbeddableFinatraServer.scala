/*
 * Copyright (C) 2012 Twitter Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ============================================================================
 * NOTICE: This code is heavly based on Finatra source for FinatraServer class.
 *
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
package es.tid.cosmos.infinity.server.finatra

import java.io.{File, FileNotFoundException}
import java.net.SocketAddress

import com.twitter.finagle._
import com.twitter.finagle.dispatch.SerialServerDispatcher
import com.twitter.finagle.http.{HttpMuxer, Request => FinagleRequest, Response => FinagleResponse}
import com.twitter.finagle.netty3.{Netty3Listener, Netty3ListenerTLSConfig}
import com.twitter.finagle.server.DefaultServer
import com.twitter.finagle.ssl.Ssl
import com.twitter.finatra._
import com.twitter.util.Await
import org.jboss.netty.handler.codec.http.{HttpRequest, HttpResponse}

/**
 * Convenience class to embed a Finatra Server on other JVM applications, not
 * as an standalone application.
 *
 * Note that this class has been implemented because original [[FinatraServer]]
 * was not designed properly for extension.
 */
class EmbeddableFinatraServer(cfg: FinatraServerCfg) extends FinatraServer {

  private[this] val nettyToFinagle =
    Filter.mk[HttpRequest, HttpResponse, FinagleRequest, FinagleResponse] { (req, service) =>
      service(FinagleRequest(req)) map { _.httpResponse }
    }

  private[this] lazy val service = {
    val appService  = new AppService(controllers)
    val fileService = new FileService
    val loggingFilter = new LoggingFilter

    addFilter(loggingFilter)
    addFilter(fileService)

    nettyToFinagle andThen allFilters(appService)
  }

  private[this] val codec = {
    http.Http()
      .maxRequestSize(cfg.maxRequestSize)
      .enableTracing(true)
      .server(ServerCodecConfig("httpserver", new SocketAddress{}))
      .pipelineFactory
  }

  def startSecureServer(https: String, certificatePath: String, keyPath: String) {
    val tlsConfig =
      Some(Netty3ListenerTLSConfig(() => Ssl.server(certificatePath, keyPath, null, null, null)))
    object HttpsListener extends Netty3Listener[HttpResponse, HttpRequest]("https", codec, tlsConfig = tlsConfig)
    object HttpsServer extends DefaultServer[HttpRequest, HttpResponse, HttpResponse, HttpRequest](
      "https", HttpsListener, new SerialServerDispatcher(_, _)
    )
    log.info("https server started on " + https)
    secureServer = Some(HttpsServer.serve(https, service))
  }

  def startHttpServer(http: String) {
    object HttpListener extends Netty3Listener[HttpResponse, HttpRequest]("http", codec)
    object HttpServer extends DefaultServer[HttpRequest, HttpResponse, HttpResponse, HttpRequest](
      "http", HttpListener, new SerialServerDispatcher(_, _)
    )
    log.info("http server started on " + http)
    server = Some(HttpServer.serve(http, service))
  }

  def startAdminServer(admin: String) {
    log.info("admin http server started on " + admin)
    adminServer = Some(HttpServer.serve(admin, HttpMuxer))
  }

  override def stop() {
    server map { _.close() }
    secureServer map { _.close() }
    adminServer map { _.close() }
  }

  override def start() {

    cfg.http map { startHttpServer(_) }

    cfg.admin map { startAdminServer(_) }

    for {
      certificatePath <- cfg.certificatePath
      keyPath <- cfg.keyPath
      https <- cfg.https
    } yield {
      if (!new File(certificatePath).canRead){
        val e = new FileNotFoundException("SSL Certificate not found: " + certificatePath)
        log.fatal(e, "SSL Certificate could not be read: " + certificatePath)
        throw e
      }
      if (!new File(keyPath).canRead){
        val e = new FileNotFoundException("SSL Key not found: " + keyPath)
        log.fatal(e, "SSL Key could not be read: " + keyPath)
        throw e
      }
      startSecureServer(https, certificatePath, keyPath)
    }
  }

  def await() {
    server       map { Await.ready(_) }
    adminServer  map { Await.ready(_) }
    secureServer map { Await.ready(_) }
  }
}
