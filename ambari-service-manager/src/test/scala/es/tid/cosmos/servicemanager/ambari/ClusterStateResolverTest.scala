/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.rest.{ServiceClient, Cluster}
import es.tid.cosmos.servicemanager.ambari.services._
import es.tid.cosmos.servicemanager.services.{NoParametrization, Service, ServiceCatalogue}

class ClusterStateResolverTest extends FlatSpec with MustMatchers
  with MockitoSugar with FutureMatchers with OneInstancePerTest {

  val instance = new ClusterStateResolver {}
  val cluster = mock[Cluster]
  val hdfs = mock[ServiceClient]
  val foo = mock[ServiceClient]
  val bar = mock[ServiceClient]
  val serviceNames = Seq("FOO", "BAR")

  case class FakeAmbariService(name: String, fakedRunningState: ServiceState)
      extends AmbariService with NoConfiguration {
    override val service: Service = new Service with NoParametrization {
      override val name: String = FakeAmbariService.this.name
    }
    override val components: Seq[ComponentDescription] = Seq.empty
    override def runningState: ServiceState = fakedRunningState
  }

  "The ClusterStateResolver" must "resolve the unknown state on clusters which have failed services" in {
    given(cluster.serviceNames).willReturn(ServiceCatalogue.toSeq.map(_.name))
    given(cluster.getService(any())).willReturn(Future.successful(hdfs))
    given(hdfs.state).willReturn("ERROR")
    given(hdfs.name).willReturn("HDFS")
    val resolveState_> = instance.resolveState(cluster, ConfiguratorTestHelpers.AllServices)
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
      Set(FakeAmbariService("FOO", StartedService), FakeAmbariService("BAR", StartedService))
    )
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
      Set(FakeAmbariService("FOO", StartedService), FakeAmbariService("BAR", InstalledService))
    )
    resolveState_> must eventually (be (AmbariClusterState.Running))
  }

  it must "handle the cluster disappearing mid-validation" in {
    val clusterNotFoundException = RequestException(null, "Cluster not found", StatusCode(404))
    given(cluster.serviceNames).willReturn(serviceNames)
    given(cluster.getService("FOO")).willReturn(Future.failed(clusterNotFoundException))
    given(cluster.getService("BAR")).willReturn(Future.failed(clusterNotFoundException))
    val resolveState_> = instance.resolveState(
      cluster,
      Set(FakeAmbariService("FOO", StartedService), FakeAmbariService("BAR", InstalledService))
    )
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
      Set(FakeAmbariService("FOO", StartedService))
    )
    resolveState_> must eventuallyFailWith[IllegalStateException]
  }
}
