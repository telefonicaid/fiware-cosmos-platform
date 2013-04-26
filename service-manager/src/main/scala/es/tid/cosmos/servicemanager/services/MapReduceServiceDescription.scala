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

package es.tid.cosmos.servicemanager.services


import es.tid.cosmos.servicemanager.{ConfigurationBundle, GlobalConfiguration, ServiceConfiguration}

/**
 * Representation of the Map Reduce service.
 *
 * @author adamos
 */
object MapReduceServiceDescription extends ServiceDescription {

  val name: String = "MAPREDUCE"

  val components: Seq[ComponentDescription] = Seq(
    new ComponentDescription("JOBTRACKER", isMaster = true),
    new ComponentDescription("TASKTRACKER", isMaster = false),
    new ComponentDescription("MAPREDUCE_CLIENT", isMaster = true))

  override def contributions(masterName: String) = {
    val global = GlobalConfiguration("version1", globalProperties)
    val mapred = ServiceConfiguration("mapred-site", "version1", mapredProperties(masterName))
    ConfigurationBundle(global, mapred)
  }

  private val globalProperties = Map(
    "mapred_map_tasks_max" -> "4",
    "mapred_red_tasks_max" -> "2",
    "mapred_cluster_map_mem_mb" -> "-1",
    "mapred_cluster_red_mem_mb" -> "-1",
    "mapred_cluster_max_map_mem_mb" -> "-1",
    "mapred_cluster_max_red_mem_mb" -> "-1",
    "mapred_job_map_mem_mb" -> "-1",
    "mapred_job_red_mem_mb" -> "-1",
    "mapred_child_java_opts_sz" -> "768",
    "mapreduce_userlog_retainhours" -> "24",
    "mapred_local_dir" -> "/hadoop/mapred",
    "mapred_system_dir" -> "/mapred/system",
    "mapred_jobstatus_dir" -> "file:////mapred/jobstatus",
    "mapred_user" -> "mapred",
    "mapred_hosts_exclude" -> "mapred.exclude",
    "mapred_hosts_include" -> "mapred.include",
    "task_controller" -> "org.apache.hadoop.mapred.DefaultTaskController",
    "maxtasks_per_job" -> "-1"
  )

  private def mapredProperties(masterName: String) = Map(
    "mapred.jobtracker.blacklist.fault-timeout-window" -> "180",
    "mapred.job.shuffle.input.buffer.percent" -> "0.7",
    "mapred.tasktracker.tasks.sleeptime-before-sigkill" -> "250",
    "mapred.healthChecker.script.timeout" -> "60000",
    "mapred.jobtracker.restart.recover" -> "false",
    "mapred.map.output.compression.codec" -> "org.apache.hadoop.io.compress.SnappyCodec",
    "mapreduce.jobtracker.staging.root.dir" -> "/user",
    "tasktracker.http.threads" -> "50",
    "mapred.jobtracker.retirejob.interval" -> "0",
    "mapred.map.tasks.speculative.execution" -> "false",
    "mapred.reduce.parallel.copies" -> "30",
    "mapreduce.reduce.input.limit" -> "10737418240",
    "io.sort.record.percent" -> ".2",
    "mapred.job.tracker.handler.count" -> "50",
    "mapred.queue.names" -> "default",
    "mapred.jobtracker.blacklist.fault-bucket-width" -> "15",
    "mapred.job.shuffle.merge.percent" -> "0.66",
    "mapreduce.tasktracker.group" -> "hadoop",
    "mapred.job.tracker.persist.jobstatus.active" -> "false",
    "mapred.reduce.tasks.speculative.execution" -> "false",
    "mapreduce.cluster.administrators" -> " hadoop",
    "mapred.jobtracker.retirejob.check" -> "10000",
    "mapred.task.timeout" -> "600000",
    "mapred.job.reduce.input.buffer.percent" -> "0.0",
    "mapred.healthChecker.interval" -> "135000",
    "mapred.child.root.logger" -> "INFO,TLA",
    "mapreduce.fileoutputcommitter.marksuccessfuljobs" -> "false",
    "mapred.output.compression.type" -> "BLOCK",
    "hadoop.job.history.user.location" -> "none",
    "mapred.max.tracker.blacklists" -> "16",
    "io.sort.factor" -> "100",
    "jetty.connector" -> "org.mortbay.jetty.nio.SelectChannelConnector",
    "mapred.inmem.merge.threshold" -> "1000",
    "mapred.job.reuse.jvm.num.tasks" -> "1",
    "mapred.reduce.slowstart.completed.maps" -> "0.05",
    "mapred.job.tracker.persist.jobstatus.hours" -> "1",
    "mapreduce.jobtracker.split.metainfo.maxsize" -> "50000000",
    "mapred.jobtracker.completeuserjobs.maximum" -> "0",
    "mapred.job.tracker.history.completed.location" -> "/mapred/history/done",
    "mapred.local.dir" -> "/hadoop/mapred",
    "mapred.jobtracker.taskScheduler" -> "org.apache.hadoop.mapred.CapacityTaskScheduler",
    "mapred.tasktracker.map.tasks.maximum" -> "4",
    "mapred.tasktracker.reduce.tasks.maximum" -> "2",
    "mapred.cluster.reduce.memory.mb" -> "-1",
    "mapred.job.map.memory.mb" -> "-1",
    "mapred.cluster.max.map.memory.mb" -> "-1",
    "mapred.cluster.max.reduce.memory.mb" -> "-1",
    "mapred.job.reduce.memory.mb" -> "-1",
    "mapred.hosts" -> "/etc/hadoop/mapred.include",
    "mapred.hosts.exclude" -> "/etc/hadoop/mapred.exclude",
    "mapred.healthChecker.script.path" -> "file:////mapred/jobstatus",
    "mapred.job.tracker.persist.jobstatus.dir" -> "/etc/hadoop/health_check",
    "mapred.child.java.opts" -> "-server -Xmx768m -Djava.net.preferIPv4Stack=true",
    "mapred.cluster.map.memory.mb" -> "-1",
    "io.sort.mb" -> "200",
    "io.sort.spill.percent" -> "0.9",
    "mapred.system.dir" -> "/mapred/system",
    "mapred.job.tracker" -> s"$masterName:50300",
    "mapred.job.tracker.http.address" -> s"$masterName:50030",
    "mapred.userlog.retain.hours" -> "24",
    "mapred.jobtracker.maxtasks.per.job" -> "-1",
    "mapred.task.tracker.task-controller" -> "org.apache.hadoop.mapred.DefaultTaskController",
    "mapreduce.jobtracker.kerberos.principal" -> "jt/_HOST@EXAMPLE.COM",
    "mapreduce.tasktracker.kerberos.principal" -> "tt/_HOST@EXAMPLE.COM",
    "mapreduce.jobtracker.keytab.file" -> "/etc/security/keytabs/jt.service.keytab",
    "mapreduce.tasktracker.keytab.file" -> "/etc/security/keytabs/tt.service.keytab",
    "mapreduce.history.server.embedded" -> "false",
    "mapreduce.history.server.http.address" -> s"$masterName:51111",
    "mapreduce.jobhistory.kerberos.principal" -> "jt/_HOST@EXAMPLE.COM",
    "mapreduce.jobhistory.keytab.file" -> "/etc/security/keytabs/jt.service.keytab"
  )
}
