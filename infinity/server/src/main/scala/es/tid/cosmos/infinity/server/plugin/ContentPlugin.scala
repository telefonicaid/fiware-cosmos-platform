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

package es.tid.cosmos.infinity.server.plugin

import java.net.InetSocketAddress
import scala.annotation.tailrec
import scala.concurrent.duration._

import org.apache.commons.logging.LogFactory
import org.apache.hadoop.conf.{Configuration, Configurable}
import org.apache.hadoop.hdfs.server.datanode.DataNode
import org.apache.hadoop.util.ServicePlugin

import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.authentication.cosmosapi.CosmosApiAuthenticationService
import es.tid.cosmos.infinity.server.config.{InfinityConfig, ContentServerConfig}
import es.tid.cosmos.infinity.server.content.ContentServer
import es.tid.cosmos.infinity.server.hadoop.DfsClientFactory

/** Datanode plugin to serve Infinity file content */
class ContentPlugin extends ServicePlugin with Configurable {

  import ContentPlugin._

  private val log = LogFactory.getLog(classOf[ContentPlugin])
  private var hadoopConfOpt: Option[Configuration] = None
  private var serverOpt: Option[ContentServer] = None

  override def setConf(conf: Configuration): Unit = {
    hadoopConfOpt = Some(conf)
  }

  override def getConf: Configuration =
    hadoopConfOpt.getOrElse(throw new IllegalStateException("Not yet injected with Hadoop config"))

  override def start(service: Any): Unit = service match {
    case dataNode: DataNode =>
      val config = new ContentServerConfig(pluginConfig)
      waitForDataNode(dataNode, config)
      log.info("Starting Infinity content server as a datanode plugin")
      createServer(dataNode, config)
    case other =>
      log.error(
        s"""Content plugin initialization failed: a DataNode was expected but ${service.getClass}
            | was found. Make sure you have configured it as datanode plugin instead of namenode one.
          """.stripMargin
      )
  }

  override def stop(): Unit = {
    log.info("Shutting down Infinity content plugin")
    serverOpt.foreach(_.stop())
    serverOpt = None
  }

  override def close(): Unit = stop()

  private def createServer(dataNode: DataNode, config: ContentServerConfig) = {
    val server = new ContentServer(
      clientFactory = new DfsClientFactory(dataNode, config.nameNodeRPCUrl),
      config = config,
      authService = authentication)
    server.start()
    serverOpt = Some(server)
  }

  @tailrec
  private def waitForDataNode(
      dataNode: DataNode,
      config: ContentServerConfig,
      loggingActions: Stream[Option[Any=>Unit]] = loggingStream): Unit = {
    if (dataNode.isDatanodeUp &&
      dataNode.isDatanodeFullyStarted &&
      dataNode.isConnectedToNN(
        new InetSocketAddress(config.nameNodeRPCUrl.getHost, config.nameNodeRPCUrl.getPort)))
      return
    loggingActions.head.foreach(_("Waiting for datanode to be ready..."))
    Thread.sleep(waitingPeriod.toMillis)
    waitForDataNode(dataNode, config, loggingActions = loggingActions.tail)
  }

  private lazy val pluginConfig = PluginConfig.load(getConf, InfinityConfig.HadoopKeys: _*)

  private lazy val authentication: AuthenticationService =
    CosmosApiAuthenticationService.fromConfig(pluginConfig)


  private val loggingStream: Stream[Option[Any => Unit]] = {
    def logInfo(obj: Any) = log.info(obj)
    def logWarn(obj: Any) = log.warn(obj)
    Stream.fill(5)(Some(logInfo _)) ++
      Stream.fill((warningThreshold / waitingPeriod).toInt)(None) ++
      Stream.continually(
        Stream.fill((warningFrequency / waitingPeriod).toInt - 1)(None) ++ Seq(Some(logWarn _))
      ).flatten
  }
}

object ContentPlugin {
  import scala.language.postfixOps

  private val warningThreshold = 15 minutes
  private val warningFrequency = 1 minute
  private val waitingPeriod = 1 second
}
