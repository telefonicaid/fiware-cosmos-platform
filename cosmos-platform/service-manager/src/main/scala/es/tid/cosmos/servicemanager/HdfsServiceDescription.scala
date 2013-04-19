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
package es.tid.cosmos.servicemanager

import ambari.{Service, Host, Cluster}
import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global
import net.liftweb.json._
import net.liftweb.json.JsonAST.JObject

object HdfsServiceDescription extends ServiceDescription {
  override val name: String = "HDFS"
  override val components: Seq[ComponentDescription] = Seq(
    new ComponentDescription("NAMENODE", true),
    new ComponentDescription("DATANODE", false),
    new ComponentDescription("HDFS_CLIENT", true))

  override protected def applyConfiguration(cluster: Cluster, master: Host): Future[Unit] = {
    val corePropertiesFuture = applyCoreProperties(cluster, master.name)
    val hdfsPropertiesFuture = applyHdfsProperties(cluster, master.name)
    for {
      _ <- corePropertiesFuture
      _ <- hdfsPropertiesFuture
    } yield ()
  }

  private def applyCoreProperties(cluster: Cluster, namenode: String): Future[Unit] = {
    val properties: JObject = parse( s"""{
      "ipc.client.connect.max.retries":"50",
      "ipc.client.idlethreshold":"8000",
      "webinterface.private.actions":"false",
      "fs.trash.interval":"360",
      "ipc.client.connection.maxidletime":"30000",
      "io.serializations":"org.apache.hadoop.io.serializer.WritableSerialization",
      "io.compression.codec.lzo.class":"com.hadoop.compression.lzo.LzoCodec",
      "io.file.buffer.size":"131072",
      "io.compression.codecs":"org.apache.hadoop.io.compress.GzipCodec,
      org.apache.hadoop.io.compress.DefaultCodec,
      com.hadoop.compression.lzo.LzoCodec,
      com.hadoop.compression.lzo.LzopCodec,
      org.apache.hadoop.io.compress.SnappyCodec",
      "fs.default.name":"hdfs://$namenode:8020",
      "fs.checkpoint.dir":"/hadoop/hdfs/namesecondary",
      "fs.checkpoint.period":"21600",
      "fs.checkpoint.size":"0.5",
      "fs.checkpoint.edits.dir":"/hadoop/hdfs/namesecondary"
    }""") match {
      case x: JObject => x
      case _ => throw new Error
    }
    cluster.applyConfiguration("core-site", "version1", properties)
  }

  private def applyHdfsProperties(cluster: Cluster, namenode: String): Future[Unit] = {
    val properties: JObject = parse( s"""{
      "dfs.web.ugi":"gopher,gopher",
      "dfs.namenode.handler.count":"100",
      "dfs.balance.bandwidthPerSec":"6250000",
      "ipc.server.max.response.size":"5242880",
      "dfs.datanode.ipc.address":"0.0.0.0:8010",
      "dfs.datanode.du.pct":"0.85f",
      "dfs.heartbeat.interval":"3",
      "dfs.datanode.max.xcievers":"4096",
      "dfs.access.time.precision":"0",
      "dfs.permissions.supergroup":"hdfs",
      "dfs.safemode.threshold.pct":"1.0f",
      "dfs.blockreport.initialDelay":"120",
      "dfs.cluster.administrators":" hdfs",
      "dfs.datanode.socket.write.timeout":"0",
      "dfs.block.access.token.enable":"true",
      "dfs.https.port":"50470",
      "dfs.block.size":"134217728",
      "dfs.replication.max":"50",
      "dfs.secondary.https.port":"50490",
      "dfs.permissions":"true",
      "dfs.umaskmode":"077",
      "ipc.server.read.threadpool.size":"5",
      "dfs.name.dir":"/hadoop/hdfs/namenode",
      "dfs.webhdfs.enabled":"false",
      "dfs.datanode.failed.volumes.tolerated":"0",
      "dfs.block.local-path-access.user":"hbase",
      "dfs.data.dir":"/hadoop/hdfs/data",
      "dfs.hosts.exclude":"/etc/hadoop/dfs.exclude",
      "dfs.hosts":"/etc/hadoop/dfs.include",
      "dfs.replication":"3",
      "dfs.datanode.address":"0.0.0.0:50010",
      "dfs.datanode.http.address":"0.0.0.0:50075",
      "dfs.http.address":"$namenode:50070",
      "dfs.datanode.du.reserved":"1",
      "dfs.namenode.kerberos.principal":"nn/_HOST@EXAMPLE.COM",
      "dfs.secondary.namenode.kerberos.principal":"nn/_HOST@EXAMPLE.COM",
      "dfs.namenode.kerberos.https.principal":"host/_HOST@EXAMPLE.COM",
      "dfs.secondary.namenode.kerberos.https.principal":"host/_HOST@EXAMPLE.COM",
      "dfs.secondary.http.address":"$namenode:50090",
      "dfs.web.authentication.kerberos.keytab":"/etc/security/keytabs/spnego.service.keytab",
      "dfs.datanode.kerberos.principal":"dn/_HOST@EXAMPLE.COM",
      "dfs.namenode.keytab.file":"/etc/security/keytabs/nn.service.keytab",
      "dfs.secondary.namenode.keytab.file":"/etc/security/keytabs/nn.service.keytab",
      "dfs.datanode.keytab.file":"/etc/security/keytabs/dn.service.keytab",
      "dfs.https.address":"$namenode:50470",
      "dfs.datanode.data.dir.perm":"750"
    }""") match {
      case x: JObject => x
      case _ => throw new Error
    }
    cluster.applyConfiguration("hdfs-site", "version1", properties)
  }
}
