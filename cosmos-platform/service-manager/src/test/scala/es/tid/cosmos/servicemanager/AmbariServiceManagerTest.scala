package es.tid.cosmos.servicemanager

import org.scalatest.{OneInstancePerTest, FlatSpec}
import es.tid.cosmos.servicemanager.ambari._
import es.tid.cosmos.platform.manager.ial._
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future
import scala.util.Try
import es.tid.cosmos.platform.manager.ial.MachineState
import org.scalatest.mock.MockitoSugar
import org.mockito.BDDMockito.given
import org.mockito.Matchers.{eq => isEq}
import org.mockito.Matchers.any
import org.mockito.Mockito.{times, verify, verifyNoMoreInteractions}
import es.tid.cosmos.platform.manager.ial.MachineState

/**
 * @author adamos
 */
class AmbariServiceManagerTest extends FlatSpec with OneInstancePerTest with MockitoSugar {

  val provisioner = mock[ProvisioningServer]
  val infrastructureProvider = mock[InfrastructureProvider]
  val cluster = mock[Cluster]
  val hdfs = mock[Service]
  val mapreduce = mock[Service]
  val instance = new AmbariServiceManager(provisioner, infrastructureProvider)


  def waitForClusterCompletion(id: ClusterId, sm: ServiceManager): ClusterState = {
    val description = sm.describeCluster(id)
    description.get.state match {
      case Provisioning => {
        Thread.sleep(1000)
        waitForClusterCompletion(id, sm)
      }
      case Failed(reason) => throw reason
      case _ => description.get.state
    }
  }

  "A ServiceManager" should "have no Cluster IDs by default" in {
    assert(instance.clusterIds.isEmpty)
  }

  //TODO: Test the component division between master and slave

  it should "create a cluster: size 1, HDFS+MAPREDUCE, when with 1 Medium Running machine" in {
    val machines = machinesOf(1)
    val hosts = hostsOf(1)
    setExpectations(machines, hosts)
    val clusterId = instance.createCluster("clusterName", 1)
    val state = waitForClusterCompletion(clusterId, instance)
    verify(hosts.head, times(4)).addComponents(any())
    doVerifications(machines)
    assert(clusterId != null)
    assert(state === Running)
  }

  it should "create a cluster: size 3, HDFS+MAPREDUCE, when with 3 Medium Running machines (1 Master, 2 Slaves)" in {
    val machines = machinesOf(3)
    val hosts = hostsOf(3)
    setExpectations(machines, hosts)
    val clusterId = instance.createCluster("clusterName", 3)
    val state = waitForClusterCompletion(clusterId, instance)
    hosts.foreach(host => verify(host, times(2)).addComponents(any())) // 2 for Master{HDFS, MAPREDUCE) + 2 for Slave{HDFS, MAPREDUCE} components
    doVerifications(machines)
    assert(clusterId != null)
    assert(state === Running)
  }

  def setExpectations(machines: Seq[MachineState], hosts: Seq[Host]) {
    given(infrastructureProvider.createMachines(any(), any(), any()))
      .willReturn(Try(machines.map(machine => fakeFuture(machine))))
    given(provisioner.createCluster(any(), any())).willReturn(fakeFuture(cluster))
    given(cluster.applyConfiguration(any(), any(), any())).willReturn(fakeFuture())
    machines.zip(hosts).foreach{case (machine, host) =>
      given(cluster.addHost(machine.hostname)).willReturn(fakeFuture(host))}
    given(cluster.addService("HDFS")).willReturn(fakeFuture(hdfs))
    given(cluster.addService("MAPREDUCE")).willReturn(fakeFuture(mapreduce))
    given(hdfs.install()).willReturn(fakeFuture(hdfs))
    given(mapreduce.install()).willReturn(fakeFuture(mapreduce))
    given(hdfs.start()).willReturn(fakeFuture(hdfs))
    given(mapreduce.start()).willReturn(fakeFuture(mapreduce))
    given(hdfs.addComponent(any())).willReturn(fakeFuture("A Service1 Component"))
    given(mapreduce.addComponent(any())).willReturn(fakeFuture("A Service2 Component"))
    hosts.foreach(host => given(host.addComponents(any())).willReturn(fakeFuture()))
  }

  def doVerifications(machines: Seq[MachineState]) {
    verify(infrastructureProvider).createMachines("clusterName", MachineProfile.M, machines.size)
    verify(provisioner).createCluster("clusterName", "HDP-1.2.0")
    verify(cluster).applyConfiguration(isEq("global"), isEq("version1"), any())
    verify(cluster).applyConfiguration(isEq("core-site"), isEq("version1"), any())
    verify(cluster).applyConfiguration(isEq("hdfs-site"), isEq("version1"), any())
    verify(cluster).applyConfiguration(isEq("mapred-site"), isEq("version1"), any())
    machines.foreach(m => verify(cluster).addHost(m.hostname))
    verify(cluster).addService("HDFS")
    verify(cluster).addService("MAPREDUCE")
    verify(hdfs).addComponent("NAMENODE")
    verify(hdfs).addComponent("DATANODE")
    verify(hdfs).addComponent("HDFS_CLIENT")
    verify(mapreduce).addComponent("JOBTRACKER")
    verify(mapreduce).addComponent("TASKTRACKER")
    verify(mapreduce).addComponent("MAPREDUCE_CLIENT")
    verify(hdfs).install()
    verify(hdfs).start()
    verify(mapreduce).install()
    verify(mapreduce).start()
    verifyNoMoreInteractions(infrastructureProvider, provisioner, cluster, hdfs, mapreduce)
  }

  def machinesOf(numberOfMachines: Int): Seq[MachineState] = (1 to numberOfMachines).map(number =>
    MachineState(
      new Id(s"ID$number"), s"aMachineName$number",
      MachineProfile.M, MachineStatus.Running,
      s"hostname$number", s"ipAddress$number"))

  def hostsOf(numberOfHosts: Int): Seq[Host] = (1 to numberOfHosts).map(_ => mock[Host])

  def fakeFuture[T](thing: T) = Future.successful(thing)
}
