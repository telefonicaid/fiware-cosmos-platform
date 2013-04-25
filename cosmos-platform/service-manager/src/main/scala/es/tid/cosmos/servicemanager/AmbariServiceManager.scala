
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

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.{Await, Future}
import scala.concurrent.duration.Duration
import ambari._
import net.liftweb.json._
import net.liftweb.json.JsonAST.JObject
import es.tid.cosmos.platform.manager.ial.{InfrastructureProvider, MachineProfile, MachineState}
import es.tid.cosmos.servicemanager.services.{ServiceDescription, MapReduceServiceDescription, HdfsServiceDescription}

class AmbariServiceManager(provisioner: ProvisioningServer,  infrastructureProvider: InfrastructureProvider)
  extends ServiceManager {

  @volatile var clusters = Map[ClusterId, MutableClusterDescription]()

  override def clusterIds: Seq[ClusterId] = clusters.keys.toSeq

  override def describeCluster(id: ClusterId): Option[ClusterDescription] = clusters.get(id).map(_.getView)

  /**
   * Wait until all pending operations are finished
   */
  def close() { clusters.values.foreach(description => Await.ready(description.deployment, Duration.Inf))}

  override def createCluster(name: String, clusterSize: Int): ClusterId = {
    val machineFutures: List[Future[MachineState]] =
      infrastructureProvider.createMachines(name, MachineProfile.M, clusterSize).get.toList
    val clusterFuture: Future[Cluster] = applyGlobalConfiguration(provisioner.createCluster(name = name, version = "HDP-1.2.0"))
    val hostFutures: List[Future[Host]] = addHosts(machineFutures, clusterFuture)
    val serviceFutures: List[Future[Service]] = List(HdfsServiceDescription, MapReduceServiceDescription)
      .map(createService(clusterFuture, hostFutures, _))
    val deployedClusterFuture: Future[List[Service]] = installInOrder(serviceFutures)

    val id = new ClusterId
    val description = new MutableClusterDescription(id, name, clusterSize, deployedClusterFuture)
    clusters.synchronized {
      clusters = clusters.updated(id, description)
    }
    id
  }

  override def terminateCluster(id: ClusterId) {}

  private def createService(clusterFuture: Future[Cluster], hostFutures: List[Future[Host]],
                            serviceDescription: ServiceDescription): Future[Service] = {
    for {
      cluster <- clusterFuture
      hosts <- Future.sequence(hostFutures)
      service <- serviceDescription.createService(cluster, masterAndSlaves(hosts).head, masterAndSlaves(hosts).tail)
    } yield service
  }

  private def masterAndSlaves(hosts: Seq[Host]): Seq[Host] = hosts match {
    case oneHost::Nil => oneHost::List(oneHost)
    case first::rest => hosts
    case _ => throw new IllegalArgumentException("Need at least one host")
  }

  private def installInOrder(serviceFutures: List[Future[Service]]): Future[List[Service]] = {
    def doInstall(installedServicesFuture: Future[List[Service]], serviceFuture: Future[Service]): Future[List[Service]] = {
      for {
        installedServices <- installedServicesFuture
        service <- installAndStart(serviceFuture)
      } yield {
        service::installedServices
      }
    }
    serviceFutures.foldLeft(Future{List[Service]()})(doInstall)
  }

  private def installAndStart(serviceFuture: Future[Service]): Future[Service] = {
    for {
      service <- serviceFuture
      installedService <- service.install
      startedService <- installedService.start
    } yield startedService
  }

  private def applyGlobalConfiguration(clusterFuture: Future[Cluster]): Future[Cluster] = {
    for {
      cluster <- clusterFuture
      _ <- applyGlobalProperties(cluster)
    } yield cluster
  }

  private def addHosts(machineFutures: List[Future[MachineState]], clusterFuture: Future[Cluster]): List[Future[Host]] = {
    for {
      machineFuture <- machineFutures
    } yield for {
      cluster <- clusterFuture
      machine <- machineFuture
      host <- cluster.addHost(machine.hostname)
    } yield host
  }

  private def applyGlobalProperties(cluster: Cluster): Future[Unit] = {
    val properties: JObject = parse(
      """{
        "dfs_name_dir":"/hadoop/hdfs/namenode",
        "fs_checkpoint_dir":"/hadoop/hdfs/namesecondary",
        "dfs_data_dir":"/hadoop/hdfs/data",
        "hdfs_log_dir_prefix":"/var/log/hadoop",
        "hadoop_pid_dir_prefix":"/var/run/hadoop",
        "dfs_webhdfs_enabled":false,
        "hadoop_heapsize":"1024",
        "namenode_heapsize":"1024m",
        "namenode_opt_newsize":"200m",
        "namenode_opt_maxnewsize":"640m",
        "datanode_du_reserved":"1",
        "dtnode_heapsize":"1024m",
        "dfs_datanode_failed_volume_tolerated":"0",
        "fs_checkpoint_period":"21600",
        "fs_checkpoint_size":"0.5",
        "dfs_exclude":"dfs.exclude",
        "dfs_include":"dfs.include",
        "dfs_replication":"3",
        "dfs_block_local_path_access_user":"hbase",
        "dfs_datanode_data_dir_perm":"750",
        "security_enabled":false,
        "kerberos_domain":"EXAMPLE.COM",
        "kadmin_pw":"",
        "keytab_path":"/etc/security/keytabs",
        "namenode_formatted_mark_dir":"/var/run/hadoop/hdfs/namenode/formatted/",
        "hcat_conf_dir":"",
        "mapred_local_dir":"/hadoop/mapred",
        "mapred_system_dir":"/mapred/system",
        "scheduler_name":"org.apache.hadoop.mapred.CapacityTaskScheduler",
        "jtnode_opt_newsize":"200m",
        "jtnode_opt_maxnewsize":"200m",
        "jtnode_heapsize":"1024m",
        "mapred_map_tasks_max":"4",
        "mapred_red_tasks_max":"2",
        "mapred_cluster_map_mem_mb":"-1",
        "mapred_cluster_red_mem_mb":"-1",
        "mapred_cluster_max_map_mem_mb":"-1",
        "mapred_cluster_max_red_mem_mb":"-1",
        "mapred_job_map_mem_mb":"-1",
        "mapred_job_red_mem_mb":"-1",
        "mapred_child_java_opts_sz":"768",
        "io_sort_mb":"200",
        "io_sort_spill_percent":"0.9",
        "mapreduce_userlog_retainhours":"24",
        "maxtasks_per_job":"-1",
        "lzo_enabled":false,
        "snappy_enabled":true,
        "rca_enabled":true,
        "mapred_hosts_exclude":"mapred.exclude",
        "mapred_hosts_include":"mapred.include",
        "mapred_jobstatus_dir":"file:////mapred/jobstatus",
        "task_controller":"org.apache.hadoop.mapred.DefaultTaskController",
        "hbase_conf_dir":"/etc/hbase",
        "proxyuser_group":"users",
        "dfs_datanode_address":"50010",
        "dfs_datanode_http_address":"50075",
        "gpl_artifacts_download_url":"",
        "apache_artifacts_download_url":"",
        "ganglia_runtime_dir":"/var/run/ganglia/hdp",
        "gmetad_user":"nobody",
        "gmond_user":"nobody",
        "java64_home":"/usr/jdk/jdk1.6.0_31",
        "run_dir":"/var/run/hadoop",
        "hadoop_conf_dir":"/etc/hadoop",
        "hdfs_user":"hdfs",
        "mapred_user":"mapred",
        "hbase_user":"hbase",
        "hive_user":"hive",
        "hcat_user":"hcat",
        "webhcat_user":"hcat",
        "oozie_user":"oozie",
        "zk_user":"zookeeper",
        "smokeuser":"ambari-qa",
        "user_group":"hadoop",
        "rrdcached_base_dir":"/var/lib/ganglia/rrds"
      }""") match {
      case x: JObject => x
      case _ => throw new Error
    }
    cluster.applyConfiguration("global", "version1", properties)
  }
}
