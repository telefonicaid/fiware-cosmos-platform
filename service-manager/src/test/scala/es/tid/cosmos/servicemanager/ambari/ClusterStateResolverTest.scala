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

import scala.concurrent.Future

import dispatch.StatusCode
import org.mockito.BDDMockito.given
import org.mockito.Matchers.any
import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.{ComponentDescription, RequestException}
import es.tid.cosmos.servicemanager.ambari.configuration.{ConfigurationBundle, ConfigurationKeys}
import es.tid.cosmos.servicemanager.ambari.rest.{Service, Cluster}
import es.tid.cosmos.servicemanager.ambari.services.{InstalledService, StartedService, ServiceState, AmbariServiceDescription}

class ClusterStateResolverTest extends FlatSpec with MustMatchers
  with MockitoSugar with FutureMatchers with OneInstancePerTest {

  val instance = new ClusterStateResolver {}
  val cluster = mock[Cluster]
  val hdfs = mock[Service]
  val foo = mock[Service]
  val bar = mock[Service]
  val serviceNames = Seq("FOO", "BAR")

  case class FakeAmbariServiceDescription(
      override val name: String,
      override val runningState: ServiceState) extends AmbariServiceDescription {
    def contributions(properties: Map[ConfigurationKeys.Value, String]): ConfigurationBundle =
      throw new NotImplementedError()
    val components: Seq[ComponentDescription] = Seq()
  }

  "The ClusterStateResolver" must "resolve the unknown state on clusters which have failed services" in {
    given(cluster.serviceNames).willReturn(AmbariServiceManager.AllServices.map(_.name))
    given(cluster.getService(any())).willReturn(Future.successful(hdfs))
    given(hdfs.state).willReturn("ERROR")
    given(hdfs.name).willReturn("HDFS")
    val resolveState_> = instance.resolveState(cluster, AmbariServiceManager.AllServices)
    resolveState_> must eventually (be (AmbariClusterState.Unknown))
  }

  it must "resolve the unknown state if not all services are in the running state" in {
    given(cluster.serviceNames).willReturn(serviceNames)
    given(cluster.getService("FOO")).willReturn(Future.successful(foo))
    given(cluster.getService("BAR")).willReturn(Future.successful(bar))
    given(foo.name).willReturn("FOO")
    given(foo.state).willReturn(StartedService.toString)
    given(bar.name).willReturn("BAR")
    given(bar.state).willReturn(InstalledService.toString)
    val resolveState_> = instance.resolveState(
      cluster,
      Seq(
        FakeAmbariServiceDescription("FOO", StartedService),
        FakeAmbariServiceDescription("BAR", StartedService)))
    resolveState_> must eventually (be (AmbariClusterState.Unknown))
  }

  it must "resolve the running state if all services are in the running state" in {
    given(cluster.serviceNames).willReturn(serviceNames)
    given(cluster.getService("FOO")).willReturn(Future.successful(foo))
    given(cluster.getService("BAR")).willReturn(Future.successful(bar))
    given(foo.name).willReturn("FOO")
    given(foo.state).willReturn(StartedService.toString)
    given(bar.name).willReturn("BAR")
    given(bar.state).willReturn(InstalledService.toString)
    val resolveState_> = instance.resolveState(
      cluster,
      Seq(
        FakeAmbariServiceDescription("FOO", StartedService),
        FakeAmbariServiceDescription("BAR", InstalledService)))
    resolveState_> must eventually (be (AmbariClusterState.Running))
  }

  it must "handle the cluster disappearing mid-validation" in {
    val clusterNotFoundException = RequestException(null, "Cluster not found", StatusCode(404))
    given(cluster.serviceNames).willReturn(serviceNames)
    given(cluster.getService("FOO")).willReturn(Future.failed(clusterNotFoundException))
    given(cluster.getService("BAR")).willReturn(Future.failed(clusterNotFoundException))
    val resolveState_> = instance.resolveState(
      cluster,
      Seq(
        FakeAmbariServiceDescription("FOO", StartedService),
        FakeAmbariServiceDescription("BAR", InstalledService)))
    resolveState_> must eventually (be (AmbariClusterState.ClusterNotPresent))
  }

  it must "throw an exception if an unknown service is found" in {
    given(cluster.serviceNames).willReturn(serviceNames)
    given(cluster.getService("FOO")).willReturn(Future.successful(foo))
    given(cluster.getService("BAR")).willReturn(Future.successful(bar))
    given(foo.name).willReturn("FOO")
    given(foo.state).willReturn(StartedService.toString)
    given(bar.name).willReturn("BAR")
    given(bar.state).willReturn(InstalledService.toString)
    val resolveState_> = instance.resolveState(
      cluster,
      Seq(
        FakeAmbariServiceDescription("FOO", StartedService)))
    resolveState_> must eventuallyFailWith[IllegalStateException]
  }
}
