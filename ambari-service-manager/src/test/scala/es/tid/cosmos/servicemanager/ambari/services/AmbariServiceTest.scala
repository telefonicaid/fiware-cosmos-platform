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

package es.tid.cosmos.servicemanager.ambari.services

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.services.Service

class AmbariServiceTest extends FlatSpec with MustMatchers {

  class AmbariTestService(
      serverServices: Seq[String] = Seq.empty,
      clientServices: Seq[String] = Seq.empty)
    extends AmbariService with NoConfiguration {

    override val service: Service = null

    override val components: Seq[ComponentDescription] =
      serverServices.map(ComponentDescription.masterComponent) ++
      clientServices.map(ComponentDescription.masterComponent).map(_.makeClient)
  }

  it must "be considered running for state 'INSTALL' when its components are of client type" in {
    val description = new AmbariTestService(clientServices = Seq("component1", "component2"))
    description.runningState must be (InstalledService)
  }

  it must "be considered running for state 'STARTED' in any other case" in {
    val description = new AmbariTestService(
      serverServices = Seq("component1"), clientServices = Seq("component2")
    )
    description.runningState must be (StartedService)
  }
}
