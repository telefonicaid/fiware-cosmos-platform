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

package es.tid.cosmos.servicemanager.ambari

import java.util.concurrent.Executors
import scala.concurrent.{ExecutionContext, Future}
import scala.util.{Success, Failure}

import akka.actor.{ActorRef, Actor}

import es.tid.cosmos.platform.ial.{InfrastructureProvider, MachineProfile, MachineState}
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.machines._
import es.tid.cosmos.servicemanager.ambari.services._
import es.tid.cosmos.servicemanager.ambari.rest.{ClusterProvisioner, Service, Cluster}
import es.tid.cosmos.servicemanager.ambari.configuration.FileConfigurationContributor
import es.tid.cosmos.servicemanager.util.TcpServer
import es.tid.cosmos.servicemanager.util.TcpServer._
import scala.util.control.NonFatal

class ClusterFactory(
    infraProvider: InfrastructureProvider,
    provisioner: ClusterProvisioner) extends Actor with FileConfigurationContributor {

  import ClusterFactory._

  override val configName = "global-basic"

  def receive = {
    case msg: CreateClusterMsg => withExceptionAsReply {
      sender ! ClusterCreatedMsg(createCluster(msg))
    }
    case TerminateClusterMsg(cluster) => withExceptionAsReply {
      // Need to bind the current sender to a value to be used when cluster termination is
      // resolved. Otherwise a call to 'sender' would return an invalid actor reference.
      val requester = sender
      terminateCluster(cluster).onComplete {
        case Success(()) => requester ! ClusterTerminatedMsg(cluster.id)
        case Failure(error) => requester ! akka.actor.Status.Failure(error)
      }
    }
  }

  private def withExceptionAsReply(action: => Unit) = {
    try {
      action
    } catch {
      case NonFatal(e)  =>
        sender ! akka.actor.Status.Failure(e)
        throw e
    }
  }

  private def createCluster(msg: CreateClusterMsg) = {
    val (master_>, slaves_>) = createMachines(msg.masterProfile, msg.slavesProfile, msg.clusterSize)
    val machines_> = for {
      master <- master_>
      slaves <- slaves_>
    } yield if (master == slaves.head) slaves else master +: slaves
    val deployment_> = for {
      master <- master_>
      slaves <- slaves_>
      deployment <- deployCluster(
        msg.id, msg.name, msg.serviceDescriptions, master, slaves, msg.hadoopConfig)
    } yield deployment
    val nameNode_> = master_>.map(toNameNodeUri)
    new MutableClusterDescription(
      msg.id, msg.name, msg.clusterSize, deployment_>, machines_>, nameNode_>)
  }

  private def createMachines(
      masterProfile: MachineProfile.Value,
      slavesProfile: MachineProfile.Value,
      numberOfMachines: Int): (Future[MachineState], Future[Seq[MachineState]]) = {
    if (masterProfile == slavesProfile) {
      val x = for {
        machines <- infraProvider.createMachines(masterProfile, numberOfMachines, waitForSsh)
      } yield masterAndSlaves(machines)
      (x.map(_._1), x.map(_._2))
    } else {
      val master_> = infraProvider.createMachines(masterProfile, 1, waitForSsh).map(_.head)
      val slaves_> =
        if (numberOfMachines > 1)
          infraProvider.createMachines(slavesProfile, numberOfMachines - 1, waitForSsh)
        else throw new IllegalArgumentException("Machine creation request with 1 server " +
          "indicating different profiles for master and slaves")
      (master_>, slaves_>)
    }
  }

  private def deployCluster(
      id: ClusterId,
      name: String,
      serviceDescriptions: Seq[AmbariServiceDescription],
      master : MachineState,
      slaves: Seq[MachineState],
      hadoopConfig: HadoopConfig) = {
    for {
      cluster <- initCluster(id, master +: slaves)
      masterHost <- cluster.addHost(master.hostname)
      nonMasterHosts <- cluster.addHosts(slaves
        .filterNot(_.hostname == master.hostname)
        .map(_.hostname))
      slaveHosts = if (nonMasterHosts.length < slaves.length)
        masterHost +: nonMasterHosts else nonMasterHosts
      configuredCluster <- Configurator.applyConfiguration(
        cluster,
        masterHost,
        slaveHosts,
        hadoopConfig.mappersPerSlave,
        hadoopConfig.reducersPerSlave,
        this +: serviceDescriptions)
      services <- Future.traverse(serviceDescriptions)(
        srv => srv.createService(configuredCluster, masterHost, slaveHosts))
      deployedServices <- installInOrder(services)
    } yield deployedServices
  }

  private def waitForSsh(state: MachineState): Future[Unit] =
    TcpServer(state.hostname, SshService).waitForServer()

  private def installInOrder(services: Seq[Service]): Future[Seq[Service]] = {
    def doInstall(
        installedServices_> : Future[Seq[Service]],
        service : Service): Future[Seq[Service]] = for {
      installedServices <- installedServices_>
      service <- installAndStart(service)
    } yield service +: installedServices
    services.foldLeft(Future.successful(Seq[Service]()))(doInstall)
  }

  private def installAndStart(service : Service): Future[Service] = {
    for {
      installedService <- service.install()
      startedService <- installedService.start()
    } yield startedService
  }

  private def initCluster(id: ClusterId, machines: Seq[MachineState]) : Future[Cluster] = {
    val distinctHostnames = machines.map(_.hostname).distinct
    for {
      _ <- provisioner.bootstrapMachines(distinctHostnames, infraProvider.rootPrivateSshKey)
      _ <- ensureHostsRegistered(distinctHostnames)
      cluster <- provisioner.createCluster(id.toString, stackVersion)
    } yield cluster
  }

  private def ensureHostsRegistered(hostnames: Seq[String]): Future[Unit] =
    provisioner.registeredHostnames.flatMap(registeredHostnames =>
      if (hostnames.diff(registeredHostnames).nonEmpty) {
        Thread.sleep(1000)
        ensureHostsRegistered(hostnames)
      } else
        Future.successful())

  private def terminateCluster(cluster: MutableClusterDescription): Future[Unit] = {
    val clusterState = cluster.state
    require(!Seq(Provisioning, Terminating, Terminated).contains(clusterState),
      s"Cluster $cluster.id is in state $clusterState, which is not a valid state for termination")
    val termination_> = for {
      _ <- removeClusterFromAmbari(cluster)
      machines <- cluster.machines_>
      _ <- infraProvider.releaseMachines(machines)
    } yield ()
    cluster.signalTermination(termination_>)
    termination_>
  }

  private def removeClusterFromAmbari(cluster: MutableClusterDescription) = for {
    _ <- stopStartedServices(cluster.id)
    _ <- provisioner.removeCluster(cluster.id.toString)
    machines <- cluster.machines_>
    distinctHostnames = machines.map(_.hostname).distinct
    _ <- provisioner.teardownMachines(distinctHostnames, infraProvider.rootPrivateSshKey)
  } yield ()

  private def stopStartedServices(id: ClusterId) = for {
    cluster <- provisioner.getCluster(id.toString)
    services <- Future.traverse(cluster.serviceNames)(cluster.getService(_))
    startedServices = services.filter(_.state == "STARTED")
    stoppedServices <- Future.traverse(startedServices)(_.stop())
  } yield stoppedServices
}

object ClusterFactory {

  val stackVersion = "Cosmos-0.1.0"

  case class HadoopConfig(mappersPerSlave: Int, reducersPerSlave: Int)

  case class CreateClusterMsg(
      id: ClusterId,
      name: String,
      clusterSize: Int,
      masterProfile: MachineProfile.Value,
      slavesProfile: MachineProfile.Value,
      hadoopConfig: HadoopConfig,
      serviceDescriptions: Seq[AmbariServiceDescription])

  case class ClusterCreatedMsg(cluster: MutableClusterDescription)

  case class TerminateClusterMsg(cluster: MutableClusterDescription)

  case class ClusterTerminatedMsg(id: ClusterId)

  case class ClusterTerminationFailedMsg(id: ClusterId, error: Throwable)

  implicit val executionContext: ExecutionContext = ExecutionContext.fromExecutor(
    ExecutionContext.fromExecutorService(Executors.newFixedThreadPool(1)))
}
