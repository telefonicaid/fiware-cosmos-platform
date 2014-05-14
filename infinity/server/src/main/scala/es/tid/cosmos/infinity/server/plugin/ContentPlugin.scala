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
import scalaz._

import org.apache.commons.logging.LogFactory
import org.apache.hadoop.conf.{Configuration, Configurable}
import org.apache.hadoop.hdfs.server.datanode.DataNode
import org.apache.hadoop.util.ServicePlugin

import es.tid.cosmos.infinity.server.authentication.AuthenticationComponent
import es.tid.cosmos.infinity.server.config.{InfinityConfig, ContentServerConfig}
import es.tid.cosmos.infinity.server.content.ContentServer
import es.tid.cosmos.infinity.server.hadoop.DfsClientFactory

/** Datanode plugin to serve Infinity file content */
class ContentPlugin extends ServicePlugin with Configurable {
  this: AuthenticationComponent =>

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
      log.info("Starting Infinity content server as a datanode plugin")
      val config = new ContentServerConfig(
        PluginConfig.load(getConf, InfinityConfig.HadoopKeys: _*))
      checkDataNode(dataNode, config).fold(
        succ = (checkedNode) => createServer(checkedNode, config),
        fail = (errors) => illegalDataNodeState(errors.list)
      )
    case other =>
      log.error(
        s"""Content plugin initialization failed: a NameNode was expected but ${service.getClass}
            | was found. Make sure you have configured it as namenode plugin instead of datanode one.
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

  private def illegalDataNodeState(errors: Seq[String]): Unit =
    log.error(s"""Content plugin initialization failed:
                  | Datanode not in appropriate state: ${errors.mkString(",")}""".stripMargin)

  private def checkDataNode(
      dataNode: DataNode, config: ContentServerConfig): ValidationNel[String, DataNode] = {
    import scalaz.Scalaz._
    def failOnFalse(c: Boolean, m: String) = if (c) dataNode.success else m.failureNel

    val up = failOnFalse(dataNode.isDatanodeUp, "Datanode is not up")
    val fullyStarted = failOnFalse(dataNode.isDatanodeFullyStarted, "Datanode is not fully started")
    val connectedToNameNode = failOnFalse(
      dataNode.isConnectedToNN(
        new InetSocketAddress(config.nameNodeRPCUrl.getHost, config.nameNodeRPCUrl.getPort)),
      s"Datanode is not connected to NameNode [${config.nameNodeRPCUrl}]")

    (up |@| fullyStarted |@| connectedToNameNode) { (_, _, last) => last }
  }
}
