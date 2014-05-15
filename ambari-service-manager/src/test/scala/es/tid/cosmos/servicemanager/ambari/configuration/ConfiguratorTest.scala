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

package es.tid.cosmos.servicemanager.ambari.configuration

import org.mockito.BDDMockito.given
import org.mockito.Matchers.{any, eq => isEq, matches}
import org.mockito.Mockito.{verify, verifyNoMoreInteractions}
import org.scalatest.{FlatSpec, OneInstancePerTest}
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager.ambari.ConfiguratorTestHelpers.contributionsWithNumber
import es.tid.cosmos.servicemanager.ambari.rest.Cluster
import es.tid.cosmos.servicemanager.configuration._

class ConfiguratorTest extends FlatSpec with OneInstancePerTest with MustMatchers with MockitoSugar {

  val cluster = mock[Cluster]
  val noProperties: ConfigProperties = Map.empty

  "A Configurator" must "not apply configuration when there is none available" in {
    Configurator.applyConfiguration(cluster, noProperties, contributors = List())
    verifyNoMoreInteractions(cluster)
  }

  it must "configure using only one contributor" in {
    val contributor = mock[ConfigurationContributor]
    given(contributor.contributions(any())).willReturn(contributionsWithNumber(1))
    Configurator.applyConfiguration(cluster, noProperties, List(contributor))
    verify(cluster).applyConfiguration(isEq(contributionsWithNumber(1).global.get), tagPattern)
    verify(cluster).applyConfiguration(isEq(contributionsWithNumber(1).core.get), tagPattern)
    verify(cluster).applyConfiguration(isEq(contributionsWithNumber(1).services(0)), tagPattern)
  }

  it must "configure using multiple non-overlapping contributors" in {
    val contributor1 = mock[ConfigurationContributor]
    val contributor2 = mock[ConfigurationContributor]
    given(contributor1.contributions(any())).willReturn(contributionsWithNumber(1))
    given(contributor2.contributions(any())).willReturn(contributionsWithNumber(2))
    Configurator.applyConfiguration(cluster, noProperties, List(contributor1, contributor2))
    verify(cluster).applyConfiguration(
      isEq(GlobalConfiguration(
        Map("someGlobalContent1" -> "somevalue1", "someGlobalContent2" -> "somevalue2"))),
      tagPattern)
    verify(cluster).applyConfiguration(
      isEq(CoreConfiguration(
        Map("someCoreContent1" -> "somevalue1", "someCoreContent2" -> "somevalue2"))),
      tagPattern)
    verify(cluster).applyConfiguration(
      isEq(ServiceConfiguration("service-site1", Map("someServiceContent1" -> "somevalue1"))),
      tagPattern)
    verify(cluster).applyConfiguration(
      isEq(ServiceConfiguration("service-site2", Map("someServiceContent2" -> "somevalue2"))),
      tagPattern)
  }

  it must "fail when more than one service configurations have the same type" in {
    val contributor1 = mock[ConfigurationContributor]
    val contributor2 = mock[ConfigurationContributor]
    val configuration1 = contributionsWithNumber(1)
    val configuration2 = contributionsWithNumber(2)
    given(contributor1.contributions(any())).willReturn(configuration1)
    given(contributor2.contributions(any())).willReturn(
      ConfigurationBundle(configuration2.global, configuration2.core, configuration1.services))
    evaluating {
      Configurator.applyConfiguration(cluster, noProperties, List(contributor1, contributor2))
    } must produce [ConfigurationConflict]
  }

  it must "fail when global configuration contributions have property conflicts" in {
    val contributor1 = mock[ConfigurationContributor]
    val contributor2 = mock[ConfigurationContributor]
    val configuration1 = contributionsWithNumber(1)
    val configuration2 = contributionsWithNumber(2)
    given(contributor1.contributions(any())).willReturn(configuration1)
    given(contributor2.contributions(any())).willReturn(
      ConfigurationBundle(configuration1.global, configuration2.core, configuration2.services))
    evaluating {
      Configurator.applyConfiguration(cluster, noProperties, List(contributor1, contributor2))
    } must produce [ConfigurationConflict]
  }

  def tagPattern = matches("version\\d+")
}
