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

package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.ComponentDescription
import es.tid.cosmos.servicemanager.ambari.configuration._

/**
 * Representation of the HDFS service.
 */
object HdfsServiceDescription extends AmbariServiceDescription {

  override val name: String = "HDFS"

  override val components: Seq[ComponentDescription] = Seq(
    new ComponentDescription("NAMENODE", isMaster = true),
    new ComponentDescription("DATANODE", isMaster = false),
    new ComponentDescription("HDFS_CLIENT", isMaster = true))

  override def contributions(namenode: String) = {
    val global = GlobalConfiguration("version1", globalProperties)
    val core = CoreConfiguration("version1", coreProperties(namenode))
    val hdfs = ServiceConfiguration("hdfs-site", "version1", hdfsProperties(namenode))
    ConfigurationBundle(global, core, hdfs)
  }

  private val globalProperties = Map(
    "hdfs_log_dir_prefix" -> "/var/log/hadoop",
    "dfs_name_dir" -> "/hadoop/hdfs/namenode",
    "fs_checkpoint_dir" -> "/hadoop/hdfs/namesecondary",
    "dfs_data_dir" -> "/hadoop/hdfs/data",
    "dfs_webhdfs_enabled" -> false,
    "namenode_heapsize" -> "1024m",
    "namenode_opt_newsize" -> "200m",
    "namenode_opt_maxnewsize" -> "640m",
    "namenode_formatted_mark_dir" -> "/var/run/hadoop/hdfs/namenode/formatted/",
    "datanode_du_reserved" -> "1",
    "dtnode_heapsize" -> "1024m",
    "dfs_exclude" -> "dfs.exclude",
    "dfs_include" -> "dfs.include",
    "dfs_replication" -> "3",
    "dfs_block_local_path_access_user" -> "hbase",
    "dfs_datanode_failed_volume_tolerated" -> "0",
    "dfs_datanode_data_dir_perm" -> "750",
    "dfs_datanode_address" -> "50010",
    "dfs_datanode_http_address" -> "50075",
    "fs_checkpoint_period" -> "21600",
    "fs_checkpoint_size" -> "0.5",
    "hdfs_user" -> "hdfs"
  )

  private def coreProperties(namenode: String) = Map(
    "ipc.client.connect.max.retries" -> "50",
    "ipc.client.idlethreshold" -> "8000",
    "webinterface.private.actions" -> "false",
    "fs.trash.interval" -> "360",
    "ipc.client.connection.maxidletime" -> "30000",
    "io.serializations" -> "org.apache.hadoop.io.serializer.WritableSerialization",
    "io.compression.codec.lzo.class" -> "com.hadoop.compression.lzo.LzoCodec",
    "io.file.buffer.size" -> "131072",
    "io.compression.codecs" ->
      ("org.apache.hadoop.io.compress.GzipCodec,"
        + "org.apache.hadoop.io.compress.DefaultCodec,"
        + "com.hadoop.compression.lzo.LzoCodec,"
        + "com.hadoop.compression.lzo.LzopCodec,"
        + "org.apache.hadoop.io.compress.SnappyCodec"),
    "fs.default.name" -> s"hdfs://$namenode:8020",
    "fs.checkpoint.dir" -> "/hadoop/hdfs/namesecondary",
    "fs.checkpoint.period" -> "21600",
    "fs.checkpoint.size" -> "0.5",
    "fs.checkpoint.edits.dir" -> "/hadoop/hdfs/namesecondary"
  )

  private def hdfsProperties(namenode: String) = Map(
    "dfs.web.ugi" -> "gopher,gopher",
    "dfs.namenode.handler.count" -> "100",
    "dfs.balance.bandwidthPerSec" -> "6250000",
    "ipc.server.max.response.size" -> "5242880",
    "dfs.datanode.ipc.address" -> "0.0.0.0:8010",
    "dfs.datanode.du.pct" -> "0.85f",
    "dfs.heartbeat.interval" -> "3",
    "dfs.datanode.max.xcievers" -> "4096",
    "dfs.access.time.precision" -> "0",
    "dfs.permissions.supergroup" -> "hdfs",
    "dfs.safemode.threshold.pct" -> "1.0f",
    "dfs.blockreport.initialDelay" -> "120",
    "dfs.cluster.administrators" -> " hdfs",
    "dfs.datanode.socket.write.timeout" -> "0",
    "dfs.block.access.token.enable" -> "true",
    "dfs.https.port" -> "50470",
    "dfs.block.size" -> "134217728",
    "dfs.replication.max" -> "50",
    "dfs.secondary.https.port" -> "50490",
    "dfs.permissions" -> "true",
    "dfs.umaskmode" -> "077",
    "ipc.server.read.threadpool.size" -> "5",
    "dfs.name.dir" -> "/hadoop/hdfs/namenode",
    "dfs.webhdfs.enabled" -> "false",
    "dfs.datanode.failed.volumes.tolerated" -> "0",
    "dfs.block.local-path-access.user" -> "hbase",
    "dfs.data.dir" -> "/hadoop/hdfs/data",
    "dfs.hosts.exclude" -> "/etc/hadoop/dfs.exclude",
    "dfs.hosts" -> "/etc/hadoop/dfs.include",
    "dfs.replication" -> "3",
    "dfs.datanode.address" -> "0.0.0.0:50010",
    "dfs.datanode.http.address" -> "0.0.0.0:50075",
    "dfs.http.address" -> s"$namenode:50070",
    "dfs.datanode.du.reserved" -> "1",
    "dfs.namenode.kerberos.principal" -> "nn/_HOST@EXAMPLE.COM",
    "dfs.secondary.namenode.kerberos.principal" -> "nn/_HOST@EXAMPLE.COM",
    "dfs.namenode.kerberos.https.principal" -> "host/_HOST@EXAMPLE.COM",
    "dfs.secondary.namenode.kerberos.https.principal" -> "host/_HOST@EXAMPLE.COM",
    "dfs.secondary.http.address" -> s"$namenode:50090",
    "dfs.web.authentication.kerberos.keytab" -> "/etc/security/keytabs/spnego.service.keytab",
    "dfs.datanode.kerberos.principal" -> "dn/_HOST@EXAMPLE.COM",
    "dfs.namenode.keytab.file" -> "/etc/security/keytabs/nn.service.keytab",
    "dfs.secondary.namenode.keytab.file" -> "/etc/security/keytabs/nn.service.keytab",
    "dfs.datanode.keytab.file" -> "/etc/security/keytabs/dn.service.keytab",
    "dfs.https.address" -> s"$namenode:50470",
    "dfs.datanode.data.dir.perm" -> "750"
  )
}
