
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

import es.tid.cosmos.platform.manager.ial.{InfrastructureProvider, MachineProfile, MachineState}
import es.tid.cosmos.servicemanager.services.ServiceDescription
import es.tid.cosmos.servicemanager.ambari.{Service, Host, Cluster, ClusterProvisioner}

/**
 * Manager of the Ambari service configuration workflow.
 * It allows creating clusters with specified services using Ambari.
 *
 * @param provisioner the cluster provisioner
 * @param infrastructureProvider the host-machines provider
 *
 * @author ximo, adamos
 */
class AmbariServiceManager(
    provisioner: ClusterProvisioner,
    infrastructureProvider: InfrastructureProvider)
  extends ServiceManager with ConfigurationContributor {

  @volatile var clusters = Map[ClusterId, MutableClusterDescription]()

  override def clusterIds: Seq[ClusterId] = clusters.keys.toSeq

  override def describeCluster(id: ClusterId): Option[ClusterDescription] =
    clusters.get(id).map(_.getView)

  /**
   * Wait until all pending operations are finished
   */
  def close() {
    clusters.values.foreach(description => Await.ready(description.deployment, Duration.Inf))
  }

  override def createCluster(
    name: String,
    clusterSize: Int,
    serviceDescriptions: Seq[ServiceDescription]): ClusterId = {
    val machineFutures: List[Future[MachineState]] =
      infrastructureProvider.createMachines(name, MachineProfile.M, clusterSize).get.toList
    val clusterFuture: Future[Cluster] =
      provisioner.createCluster(name = name, version = "HDP-1.2.0")
    val (master: Future[Host], slaves: List[Future[Host]]) =
      masterAndSlaves(addHosts(machineFutures, clusterFuture))
    val configuredCluster: Future[Cluster] = applyConfiguration(
      clusterFuture, masterFuture = master, this::serviceDescriptions.toList)
    val serviceFutures: List[Future[Service]] =
      serviceDescriptions.map(createService(configuredCluster, master, slaves, _)).toList
    val deployedClusterFuture: Future[List[Service]] = installInOrder(serviceFutures)

    val id = new ClusterId
    val description = new MutableClusterDescription(id, name, clusterSize, deployedClusterFuture)
    clusters.synchronized {
      clusters = clusters.updated(id, description)
    }
    id
  }

  override def terminateCluster(id: ClusterId): Future[Unit] = {
    throw new UnsupportedOperationException("terminate cluster operation not implemented")
  }

  private def createService(
    clusterFuture: Future[Cluster],
    masterFuture: Future[Host],
    slaveFutures: List[Future[Host]],
    serviceDescription: ServiceDescription): Future[Service] = {
    for {
      cluster <- clusterFuture
      master <- masterFuture
      slaves <- Future.sequence(slaveFutures)
      service <- serviceDescription.createService(cluster, master, slaves)
    } yield service
  }

  private def masterAndSlaves(hosts: List[Future[Host]]) = hosts match {
    case oneHost :: Nil => (oneHost, List(oneHost))
    case first :: rest => (first, rest)
    case _ => throw new IllegalArgumentException("Need at least one host")
  }

  private def installInOrder(serviceFutures: List[Future[Service]]): Future[List[Service]] = {

    def doInstall(
      installedServicesFuture: Future[List[Service]],
      serviceFuture: Future[Service]): Future[List[Service]] = {
      for {
        installedServices <- installedServicesFuture
        service <- installAndStart(serviceFuture)
      } yield service :: installedServices
    }
    serviceFutures.foldLeft(Future.successful(List[Service]()))(doInstall)
  }

  private def installAndStart(serviceFuture: Future[Service]): Future[Service] = {
    for {
      service <- serviceFuture
      installedService <- service.install()
      startedService <- installedService.start()
    } yield startedService
  }

  private def applyConfiguration(
    clusterFuture: Future[Cluster],
    masterFuture: Future[Host],
    contributors: List[ConfigurationContributor]): Future[Cluster] = {
    for {
      cluster <- clusterFuture
      master <- masterFuture
      _ <- Configurator.applyConfiguration(cluster, master, contributors)
    } yield cluster
  }

  private def addHosts(
    machineFutures: List[Future[MachineState]],
    clusterFuture: Future[Cluster]): List[Future[Host]] = {
    for {
      machineFuture <- machineFutures
    } yield for {
      cluster <- clusterFuture
      machine <- machineFuture
      host <- cluster.addHost(machine.hostname)
    } yield host
  }

  override def contributions(masterName: String) =
    ConfigurationBundle(GlobalConfiguration("version1", globalProperties))

  private val globalProperties = Map(
      "hadoop_pid_dir_prefix" -> "/var/run/hadoop",
      "hadoop_heapsize" -> "1024",
      "security_enabled" -> false,
      "kerberos_domain" -> "EXAMPLE.COM",
      "kadmin_pw" -> "",
      "keytab_path" -> "/etc/security/keytabs",
      "hcat_conf_dir" -> "",
      "scheduler_name" -> "org.apache.hadoop.mapred.CapacityTaskScheduler",
      "jtnode_opt_newsize" -> "200m",
      "jtnode_opt_maxnewsize" -> "200m",
      "jtnode_heapsize" -> "1024m",
      "io_sort_mb" -> "200",
      "io_sort_spill_percent" -> "0.9",
      "lzo_enabled" -> false,
      "snappy_enabled" -> true,
      "rca_enabled" -> true,
      "hbase_conf_dir" -> "/etc/hbase",
      "proxyuser_group" -> "users",
      "gpl_artifacts_download_url" -> "",
      "apache_artifacts_download_url" -> "",
      "ganglia_runtime_dir" -> "/var/run/ganglia/hdp",
      "gmetad_user" -> "nobody",
      "gmond_user" -> "nobody",
      "java64_home" -> "/usr/jdk/jdk1.6.0_31",
      "run_dir" -> "/var/run/hadoop",
      "hadoop_conf_dir" -> "/etc/hadoop",
      "hbase_user" -> "hbase",
      "hive_user" -> "hive",
      "hcat_user" -> "hcat",
      "webhcat_user" -> "hcat",
      "oozie_user" -> "oozie",
      "zk_user" -> "zookeeper",
      "smokeuser" -> "ambari-qa",
      "user_group" -> "hadoop",
      "rrdcached_base_dir" -> "/var/lib/ganglia/rrds"
      )
}
